# quant

Development checks require Node.js 22+, CMake 4.2+, Ninja, and LLVM tools.
Use **clang-format 20.1.8** to match CI; clang-tidy needs complete Clang builtin
headers plus your compiler's Windows/C++ headers. LLVM's Windows distribution
provides clang-format and clang-tidy. CLion's stripped-down tools may not suffice.

Run `npm ci` once after cloning to enable Husky. Configure your Ninja build with
`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` (in CLion's CMake options, for example).
The default build directory is `cmake-build-debug`. If tools are not on PATH,
configure local paths, using your own installation directories:

```powershell
git config quant.clangFormat 'C:/path/to/LLVM/bin/clang-format.exe'
git config quant.clangTidy 'C:/path/to/LLVM/bin/clang-tidy.exe'
git config quant.buildDir 'cmake-build-debug'
```

- **Commit:** checks staged C++ files against Google formatting, including partial staging.
- **Push:** checks formatting of outgoing C++ changes, then runs clang-tidy across
  the CMake translation units when C++ or analysis configuration changes. Commit
  or stash other C++ edits first. New branches compare against `origin/master`.
- **CI:** checks formatting of changed C++ files and runs clang-tidy on Windows.
  `workflow_dispatch` checks formatting of all tracked C++ files.
- Checks never rewrite files. Formatting/analysis errors fail; clang-tidy warnings
  stay advisory and appear in the hook/Actions output. Headers are analyzed through
  the translation units that include them.

`npm run format:check` checks all committed C++ files; `npm run tidy` runs analysis
on the working tree. To format deliberately, run `clang-format -i <file>` and review
the diff before staging. Existing unformatted files will fail when touched.

Copilot review is triggered separately by GitHub's **Automatically request Copilot
code review** repository rule, with **Review new pushes** enabled. It uses
`.github/copilot-instructions.md` for Google style, UB, ownership, and performance
guidance. Copilot review requires an eligible plan and available usage allowance.
