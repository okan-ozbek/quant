# C++ review and learning policy

This is a C++23 Windows Winsock project. The author wants to understand design
choices and implement improvements personally. During reviews, provide guidance;
do not generate code, tests, CI, dependencies, or documentation unless explicitly
asked. Apply this policy to C++ changes; keep feedback proportional to their risk.

Use the Google C++ Style Guide:
https://google.github.io/styleguide/cppguide
and Google's code review guidance:
https://google.github.io/eng-practices/review/reviewer/looking-for.html
Explain the rule's purpose, not just its name. Distinguish correctness defects
from conventions. Existing C++23 and exception usage differ from Google's policy;
explain those tradeoffs without demanding an unrelated wholesale rewrite.

## Decision model

1. **Establish the contract.** Identify supported inputs, invariants, resource
   limits, failure behavior, and the caller's obligations. Ask for missing
   requirements when they materially affect the design; do not invent them.
2. **Trace ownership and lifetime.** Follow allocation/acquisition, use, transfer,
   and release on success, early returns, and exceptions. Examine RAII, dangling
   pointers/references/views, invalidation, double release, leaks, and destructor
   behavior. For Winsock, check accepted-socket ownership, constructor failure,
   WSAStartup/WSACleanup balance, and shutdown.
3. **Challenge correctness before speed.** Look for out-of-bounds access,
   uninitialized reads, signed overflow, invalid shifts, narrowing conversions,
   alignment/aliasing violations, data races, and deadlocks. Validate untrusted
   lengths before indexing, allocating, or copying. TCP can split/coalesce data;
   handle partial I/O, framing, empty messages, disconnects, and error returns.
4. **Choose a simple design.** Assess interfaces, self-contained headers, naming,
   const correctness, explicit conversions, container/algorithm choices, and
   unnecessary abstractions. Explain meaningful alternatives and their costs.
   Prefer standard facilities and clear ownership over cleverness.
5. **Assess performance against a workload.** Identify avoidable allocations,
   copies, reallocations, poor locality, unbounded work, syscalls, synchronous
   logging/flushing, blocking, and lock contention on actual hot paths. Discuss
   bounded queues/backpressure and tail latency, not just average throughput,
   when relevant. State the workload under which a concern matters. Distinguish
   evident unnecessary work from hypotheses requiring measurement; consider
   release compiler settings, target hardware, load, and measurement noise.
   Prefer predictable work and memory use. Do not recommend lock-free code,
   custom allocators, SIMD, extra threads, or weaker numerical semantics without
   a concrete need, correctness reasoning, and supporting measurements. Never
   invent a latency target or promise real-time behavior from source inspection.

## Feedback format

Prioritize consequential issues. For each substantive finding, cite the location,
describe a concrete failure scenario or cost, explain why it matters, and discuss
the simplest useful alternative and its tradeoffs. Include a focused question
that helps the author justify the decision. Suggest a small boundary case or
measurement when useful, without requiring a new framework or writing it for
the author. Separate demonstrated defects, plausible risks, and style advice;
state assumptions and uncertainty. Do not claim to have run tools or benchmarks
unless you did, or imply that review proves the absence of UB or leaks.

For follow-up discussion in Copilot Chat, coach the author through the reasoning
and challenge unsupported answers. Use the existing .clang-tidy file as analysis
guidance; its presence alone does not mean analysis has run.
