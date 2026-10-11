# Agent Instructions

Before starting work in this repository, read and apply the
[source-sdk skill](external/skills/source-sdk/SKILL.md) to load its instructions into context.
Read its linked references as required by the task.

If `external/skills/source-sdk/SKILL.md` is missing, run this command from the repository root:

```sh
git submodule update --init --depth 1 external/skills
```

Then read the skill before continuing. If initialization fails or the skill is still missing,
report the failure before proceeding with SDK changes.

## Scope and references

These rules apply to code changes and PR reviews, including `@codex review`.
Follow more specific instructions in nested `AGENTS.md` files for their scope.
Read the relevant source-sdk references before editing or reviewing:

- [Style and naming](external/skills/source-sdk/references/style.md): all C/C++ changes.
- [Containers](external/skills/source-sdk/references/containers.md): strings, containers,
  buffers, ownership and `KeyValues3`.
- [Reverse engineering](external/skills/source-sdk/references/reverse-engineering.md):
  reconstructed declarations, ABI, vtables, offsets.
- [Schema](external/skills/source-sdk/references/schema.md): schema types, fields and metadata.
- [CMake](external/skills/source-sdk/references/cmake.md): targets, platform flags,
  game manifests and protobuf generation.
- [Workflow](external/skills/source-sdk/references/workflow.md): builds, tests and commits.

## Writing code

- Keep changes focused on the requested behavior. Preserve unrelated code, existing user
  changes, file encoding and line endings. Inspect `git status --short` before and after edits.
- Follow `.clang-format`, then the surrounding file style: tabs, Allman braces, spaces inside
  nonempty parentheses, square brackets and template arguments; empty `()`, `[]` and `<>`
  stay tight. Bind pointer/reference markers to the variable: `const char *pName`.
- Preserve include order, existing alignment and local line wrapping. Do not reformat entire
  files or rename existing symbols merely to modernize them.
- Use Source naming where applicable: `p`, `n`, `b`, `m_`, `C`, `I`, `M` and `_t`.
  Write new bit flags as `1 << n`, consistent with the style reference.
- SDK targets use C17/C++17; tests use C++23. Do not introduce newer language requirements
  into SDK targets. Use modern facilities where the surrounding subsystem uses them or the
  task requires them; avoid exceptions and RTTI-dependent designs in subsystems without them.
- Reuse project containers, string helpers, allocators and platform abstractions. Preserve
  ownership, reference counting, lifetime and fixed/stack storage behavior. Check container
  indices/handles with their validity APIs; account for iterator, pointer and index invalidation.
- Preserve public API names, exported symbols, calling conventions, field offsets, alignment,
  padding, enum values and virtual slot order. In binary-matched classes, keep exact container
  types and template arguments. Successful compilation does not prove ABI compatibility.
- Keep platform and game-branch guards precise. Consider overload ambiguity, dependencies and
  compile-time cost when adding public-header overloads or templates.
- For binary-derived changes, check for `ida-pro-mcp` and use it when available. Verify the
  target game build, module and platform. Never invent offsets, signatures or vtable indexes;
  preserve unknown slots with opaque declarations according to the reverse-engineering reference.
  If IDA is unavailable, state that limitation and rely only on repository evidence.
- Derive `schema`, `META`, `TYPEMETA` and `noschema` from the owning module's records and code
  evidence. A gap may be padding; a hidden or unsaved schema field is still a schema field.
- In CMake, use lowercase built-ins, uppercase project variables and tabs. Extend the nearest
  existing source/options list and reuse project helpers. Keep platform detection centralized
  and runtime, ABI, export-map and linker changes deliberate.
- Treat `thirdparty/` as vendored code. Do not edit vendored code, imported binaries or generated
  protobuf files without a task-specific reason.
- Write technical documentation and comments in clear English. Explain non-obvious behavior,
  ownership and compatibility constraints rather than restating code. Follow the reference's
  restrictions on reconstructed-code comments; do not add new `AMNOTE:` markers in this fork.

## Verification

Choose checks that cover the changed behavior and relevant configurations:

- Headers: targeted compilation, including standalone-header coverage where relevant;
  formatting checks alone are sufficient only for formatting-only changes.
- Implementations: the smallest relevant build target and existing focused tests.
  Add a regression test for changed behavior when it can meaningfully reproduce the bug.
- CMake/manifests: configure the relevant preset/game target and build affected targets when
  source selection, linking or generated output changes. Check JSON syntax for changed manifests.
- ABI/schema declarations: verified layout/slot/metadata evidence and compile-time checks;
  distinguish these from ordinary compilation and runtime validation.

Use the existing runner under `tests/common/` and narrow CTest runs where useful, for example
`ctest --preset Debug -R utlvector`. Run full builds when the risk warrants them or requested.
Report commands actually run, their results and checks that could not run, with reasons.
Never claim unobserved tests, platforms or binary compatibility have been verified.

## Review guidelines

When reviewing a PR, including through `@codex review`:

- Review the diff against the PR's base and read enough surrounding code, declarations and
  call sites to establish the behavior. Apply the relevant references above. Treat review as
  read-only unless fixes are explicitly requested.
- Prioritize introduced correctness defects: ABI/layout/vtable mismatches, memory safety,
  ownership/refcount errors, invalidation, incorrect bounds or integer conversions, concurrency,
  platform/game-branch regressions, and build/link/protobuf failures.
- Check compatibility on affected platforms and game targets, not just the review host.
  Trace changed public contracts to consumers before concluding that a change is safe or broken.
- Check reconstructed sizes, offsets, signatures and schema metadata against available evidence.
  Identify the module/build/platform used. Missing binary access is a verification limitation,
  not proof that a declaration is wrong; do not propose guessed replacements.
- Check adherence to the explicit style and CMake rules in changed code. Avoid personal style
  preferences, unrelated cleanup and comments about pre-existing issues unless the PR makes
  them worse. Do not demand standard-library replacements or speculative abstractions.
- Report only actionable findings supported by source, a reproducible check or binary evidence.
  Explain the triggering conditions and concrete impact; qualify findings that depend on a
  particular platform or configuration. Validate assumptions before posting a finding.
- Give each finding a concise imperative title with severity (`[P0]` critical, `[P1]` urgent,
  `[P2]` normal, `[P3]` minor), a short explanation and the smallest useful changed-line location.
  Reserve P0 for unconditional critical failures; prioritize P1/P2 defects over style-only notes.
  Combine duplicate symptoms of one root cause and suggest a fix direction when justified.
- Assess whether verification covers the changed behavior. Missing tests alone are not a bug;
  identify the specific unprotected regression or state the validation gap separately if the
  review output supports a summary. Never imply checks were run when they were not.
- If no actionable defect is found, say so when the review format allows it. Do not invent
  findings to fill a quota or treat limited verification as proof of correctness.
