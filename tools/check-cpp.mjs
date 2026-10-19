// Check only: never edit files, stage changes, or apply clang-tidy fixes.
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import path from 'node:path';

const root = path.resolve(import.meta.dirname, '..');
process.chdir(root);
const cpp = file => /\.(?:c|cc|cpp|cxx|h|hh|hpp|hxx)$/.test(file);
const sources = file => /\.(?:cc|cpp|cxx)$/.test(file);
const zero = /^0+$/;

function run(exe, args, options = {}) {
  const result = spawnSync(exe, args, { encoding: 'utf8', maxBuffer: 32 * 1024 * 1024, ...options });
  if (result.error) throw new Error(`${exe}: ${result.error.message}`);
  return result;
}

function git(...args) {
  const result = run('git', args);
  if (result.status !== 0) throw new Error(result.stderr.trim() || `git ${args.join(' ')} failed`);
  return result.stdout;
}

function config(name, fallback) {
  return run('git', ['config', '--get', `quant.${name}`]).stdout.trim() || fallback;
}

function tool(name, envName, configName, requiredVersion) {
  const exe = process.env[envName] || config(configName, name);
  const result = run(exe, ['--version']);
  if (result.status !== 0) throw new Error(result.stderr || `${name} failed`);
  if (requiredVersion && !result.stdout.includes(`version ${requiredVersion}`)) {
    throw new Error(`Use ${name} ${requiredVersion}, matching CI. Got: ${result.stdout.trim()}`);
  }
  return exe;
}

function filesFromDiff(...refs) {
  return git('diff', '--name-only', '-z', '--diff-filter=ACMR', ...refs, '--').split('\0').filter(Boolean);
}

function checkFormat(files, ref) {
  if (!files.length) {
    console.log('No changed C++ files to format-check.');
    return;
  }
  const exe = tool('clang-format', 'CLANG_FORMAT', 'clangFormat', '20.1.8');
  let failed = false;
  for (const file of files) {
    // Read the index/commit, so partially staged files are checked correctly.
    const input = ref === undefined ? readFileSync(file) : git('show', `${ref}:${file}`);
    const result = run(exe, ['--style=file', '--dry-run', '--Werror', `--assume-filename=${path.resolve(file)}`], { input });
    if (result.status !== 0) {
      process.stderr.write(`${file}\n${result.stderr}`);
      failed = true;
    }
  }
  if (failed) throw new Error('Formatting failed. Review the diagnostics, format the files yourself, then stage/commit again. No files were changed.');
  console.log(`Google formatting passed (${files.length} files).`);
}

function checkTidy() {
  const build = path.resolve(process.env.CPP_BUILD_DIR || config('buildDir', 'cmake-build-debug'));
  const database = path.join(build, 'compile_commands.json');
  if (!existsSync(database)) {
    throw new Error(`Missing ${database}. Configure CMake with Ninja and -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; set quant.buildDir or CPP_BUILD_DIR if needed.`);
  }
  const exe = tool('clang-tidy', 'CLANG_TIDY', 'clangTidy');
  const configured = run('cmake', ['-S', root, '-B', build, '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'], { stdio: 'inherit' });
  if (configured.status !== 0) throw new Error('CMake configuration failed; clang-tidy was not run.');
  const entries = JSON.parse(readFileSync(database, 'utf8'));
  const expected = git('ls-files', '-z').split('\0').filter(sources);
  if (!expected.length) throw new Error('No tracked C++ translation units found.');
  const recorded = new Set(entries.map(entry => path.resolve(entry.directory, entry.file).toLowerCase()));
  for (const file of expected) {
    if (!recorded.has(path.resolve(file).toLowerCase())) {
      throw new Error(`${file} is missing from compile_commands.json. Regenerate the CMake build directory.`);
    }
  }
  const extra = [];
  // clang-tidy needs MinGW's C++/Windows headers when the database uses GCC.
  const gcc = entries.map(entry => (entry.command || '').match(/^(?:"([^"]*g\+\+(?:\.exe)?)"|([^ ]*g\+\+(?:\.exe)?))/i)).find(Boolean);
  if (process.platform === 'win32' && gcc) {
    const compiler = gcc[1] || gcc[2];
    const query = run(compiler, ['-E', '-x', 'c++', '-', '-v'], { input: '' });
    if (query.status !== 0) throw new Error(`Cannot query MinGW headers: ${query.stderr}`);
    const includes = query.stderr.split('#include <...> search starts here:')[1]?.split('End of search list.')[0];
    if (!includes) throw new Error('Cannot determine MinGW include paths.');
    const target = run(compiler, ['-dumpmachine']);
    if (target.status !== 0) throw new Error('Cannot determine MinGW target.');
    extra.push(`--extra-arg=--target=${target.stdout.trim()}`, '--extra-arg=-nostdinc++');
    for (const line of includes.trim().split(/\r?\n/)) {
      const include = path.resolve(line.trim()).replaceAll('\\', '/');
      // Use Clang's intrinsic headers instead of GCC's builtin-only headers.
      if (/\/lib\/gcc\/[^/]+\/[^/]+\/(?:include|include-fixed)$/.test(include)) continue;
      extra.push(`--extra-arg=-isystem${include}`);
    }
  }
  let failed = false;
  for (const file of expected) {
    console.log(`clang-tidy: ${file}`);
    const result = run(exe, ['-p', build, ...extra, path.resolve(file)], { stdio: 'inherit' });
    if (result.status !== 0) failed = true;
  }
  if (failed) throw new Error('clang-tidy could not complete analysis. See the errors above.');
  console.log('clang-tidy completed. Warnings are advisory; review them above.');
}

function cleanForAnalysis() {
  const dirty = filesFromDiff('HEAD').filter(file => cpp(file) || file === 'CMakeLists.txt' || file === '.clang-tidy');
  if (dirty.length) throw new Error(`Commit or stash these changes before push analysis: ${dirty.join(', ')}`);
}

try {
  const mode = process.argv[2];
  if (mode === 'staged') {
    checkFormat(filesFromDiff('--cached').filter(cpp), '');
  } else if (mode === 'push') {
    const updates = readFileSync(0, 'utf8').trim();
    for (const line of updates ? updates.split(/\r?\n/) : []) {
      const [localRef, local, , remote] = line.split(/\s+/);
      if (!localRef.startsWith('refs/heads/')) continue;
      if (zero.test(local)) continue; // Branch deletion.
      if (local !== git('rev-parse', 'HEAD').trim()) throw new Error('Check out the branch being pushed before running C++ push checks.');
      const base = zero.test(remote) ? git('merge-base', local, 'refs/remotes/origin/master').trim() : remote;
      const changed = filesFromDiff(base, local);
      checkFormat(changed.filter(cpp), local);
      if (changed.some(file => cpp(file) || file === 'CMakeLists.txt' || file === '.clang-tidy')) {
        cleanForAnalysis();
        checkTidy();
      } else {
        console.log('No C++/analysis configuration changes; clang-tidy skipped.');
      }
    }
  } else if (mode === 'format') {
    const base = process.env.BASE_SHA ? git('merge-base', process.env.BASE_SHA, 'HEAD').trim() : undefined;
    const files = base ? filesFromDiff(base, 'HEAD') : git('ls-files', '-z').split('\0');
    checkFormat(files.filter(cpp), 'HEAD');
  } else if (mode === 'tidy') {
    checkTidy();
  } else {
    throw new Error('Use staged, push, format, or tidy.');
  }
} catch (error) {
  console.error(error.message);
  process.exitCode = 1;
}
