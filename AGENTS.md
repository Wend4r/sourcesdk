# Agent Instructions

This repository uses a Valve/Source SDK C++ style with AlliedModders-specific modernizations. Keep edits local, conservative, and consistent with the surrounding file. Do not reformat unrelated code.

## Repository Orientation

- Treat the repository root as the working directory unless the task explicitly points elsewhere.
- Prefer existing project helpers, naming conventions, container types, allocation patterns, and platform abstractions over new generic utilities.
- Source code is mainly C and C++. Build and tooling files include CMake, JSON manifests, and platform-specific SDK directories.
- Preserve compatibility with existing Source SDK code. Avoid introducing modern C++ facilities into old subsystems unless nearby code already uses them or the task requires it.
- Do not sort includes unless the touched file already follows sorted include groups. The project `.clang-format` intentionally has `SortIncludes: false`.

## Formatting

Follow `.clang-format` first, then the local file style. Important defaults:

- Use tabs for indentation. Tab width and indent width are 4.
- Use Allman braces:

```cpp
if ( bEnabled )
{
	DoWork();
}
```

- Put spaces inside parentheses for declarations, calls, expressions, and control statements:

```cpp
Foo( nArgCount );
if ( bCondition )
if constexpr ( COMPILER_CONDITION )
while ( true )
```

- Put spaces inside angle brackets for templates where the formatter supports it:

```cpp
template < size_t SIZE >
CBufferStringN< SIZE >
```

- For explicit casts in edited code, prefer the same visual spacing when practical:

```cpp
( int )( nValue )
static_cast< int >( nValue )
```

- Put spaces inside braced initializer lists and short macro bodies when they contain values or statements:

```cpp
int nValues[] = { 1, 2, 3 };
#define Assert_BSO( exp ) { if ( IsStackAllocated() ) Assert( exp ); }
```

- Function declarations, definitions, and calls do not have a space before the opening parenthesis:

```cpp
void SetValue( int nValue );
SetValue( nValue );
```

- Control statements do have a space before the opening parenthesis:

```cpp
if ( bReady )
for ( int i = 0; i < nCount; ++i )
FOR_EACH_VEC( vecArgs, i )
```

- Pointer and reference markers bind to the variable name, not the type:

```cpp
const char *pString;
CBufferString &sBuffer;
CUtlString sString;
void *pData;
```

- Keep short inline functions on one line only when that matches the surrounding code and remains readable:

```cpp
int Length() const { return m_nLength; }
```

- Keep long argument lists, template argument lists, and function-call argument lists on one line when the surrounding code already does so. Do not wrap many arguments onto separate lines only because the line is long; the project `.clang-format` uses `ColumnLimit: 0`.
- Avoid alignment-only churn. Do not realign unrelated declarations, assignments, comments, or tables.
- Keep the repository's existing line endings and encoding for the file being edited.

## Naming

- Preserve Source-style prefixes and Hungarian-like conventions in existing code:
	- `p` for pointers, for example `pString`, `pData`.
	- `n` for integer number, counts or sizes, for example `nLen`, `nCount`.
	- `b` for booleans, for example `bAllowHeapAllocation`.
	- `m_` for members, for example `m_nLength`.
	- `C` prefixes for classes, `I` prefixes for interfaces, and `_t` suffixes for many enum/struct typedef-style names.
	- `M` prefixes for compile-time metaprogramming structures and traits.
- Do not rename symbols only to modernize them. Keep public ABI/API names stable unless the task is explicitly about renaming.

## C++ Practices

- Prefer existing project types and APIs such as `CBufferString`, `CUtlString`, `CUtlBuffer`, `CUtlVector`, `CUtlLeanVector`, `Q_*`, or `V_` string helpers, `Assert`, `Move`, and platform abstraction headers when nearby code uses them.
- Be careful with ownership and allocation. Many modules have custom allocation, fixed-buffer, stack-buffer, or platform-specific lifetime expectations.
- Preserve binary layout, virtual table layout, exported symbols, calling conventions, and public header contracts.
- Avoid exceptions unless the surrounding subsystem already uses them.
- Avoid RTTI-dependent designs unless already present in that area.
- Keep platform guards precise. Do not widen `_WIN32`, POSIX, Xbox, PS3, or dedicated-server paths casually.
- When adding overloads or templates in public headers, consider compile-time cost and ABI/API ambiguity.
- Comments should explain non-obvious constraints, compatibility requirements, reverse-engineered behavior, or ownership rules. Do not add comments that restate the code.

## Reverse Engineering and Binary Work

- If the task involves signatures, offsets, vtables, calling conventions, binary compatibility, SDK/game gamedata, disassembly, or behavior reconstructed from a binary, first check whether an `ida-pro-mcp` server/tool is available among the active MCP modules.
- When `ida-pro-mcp` is available, use it as the primary source for IDA-derived facts before editing code or gamedata. Prefer direct evidence from functions, xrefs, names, types, strings, and decompiler output over guessing from memory.
- Record important IDA-derived assumptions in the final response or in a nearby technical comment when the code would otherwise be hard to justify.
- Do not invent addresses, offsets, signatures, symbol names, or vtable indexes. If IDA data is unavailable, state the limitation and use repository evidence only.
- For gamedata/signature changes, verify the target game, engine branch, platform, and binary version. Keep platform-specific entries separated and avoid broadening a signature without evidence.
- For every binary-reconstructed structure or class with a known size, add a `COMPILE_TIME_ASSERT( sizeof( TypeName ) == 0xSize );` near the declaration/definition that owns the layout. Use the size verified from the target binary, and keep platform-specific or branch-specific sizes guarded by the matching preprocessor conditions.
- When reconstructing structure fields, use string references from the binary as primary orientation points: inspect nearby string literals, xrefs, logging/assert messages, schema names, RTTI/typeinfo, and constructor/destructor references before naming fields or methods. Do not infer a semantic name from offset alone when string evidence is available.
- Preserve exact field offsets and padding for reconstructed layouts. Use explicit padding members only when the real field type or purpose is not known, and name them in a way that makes the offset range clear.
- Preserve exact virtual table slot order. Never reorder, remove, or collapse virtual methods just because their purpose is unknown. If a slot exists in the binary, the SDK declaration must keep a corresponding slot.
- Mark unknown virtual methods with the form `Unk_IntendedMethodName( void *p )` when there is a plausible intended method name but the signature is not known. Keep the placeholder in the exact vtable slot, use the project pointer style, and prefer a single opaque `void *p` parameter until the real signature is verified.
- If neither the intended method name nor the signature is known, use a slot-preserving unknown name that includes the vtable index or offset, then rename only after IDA/binary evidence supports the real meaning.
- When a method signature is partially known, do not "improve" it with guessed argument types. Keep opaque pointer/integer placeholders and document the evidence needed to replace them.

## CMake Conventions

- Project-owned CMake lives under `cmake/` and the root `CMakeLists.txt`. Treat `thirdparty/protobuf/**` CMake as vendored code; do not adjust it unless the task is explicitly about vendored protobuf.
- CMake style in this repository uses lowercase built-in commands, uppercase project variables, tabs for indentation, and multi-line `set()` / `list(APPEND ...)` blocks:

```cmake
set(SOURCESDK_TIER1_SOURCE_FILES
	${SOURCESDK_TIER1_DIR}/bitbuf.cpp
	${SOURCESDK_TIER1_DIR}/tier1.cpp
)
```

- Preserve the aggregate-list pattern. Global project lists such as `SOURCESDK_SOURCE_FILES`, `SOURCESDK_INCLUDE_DIRS`, `SOURCESDK_COMPILE_DEFINITIONS`, `SOURCESDK_LINK_LIBRARIES`, `PLATFORM_COMPILE_OPTIONS`, `PLATFORM_LINK_OPTIONS`, and `PLATFORM_COMPILE_DEFINITIONS` are intentionally accumulated across included modules.
- When adding source files, include directories, compile definitions, link options, or libraries, append to the nearest existing `SOURCESDK_<MODULE>_*`, `SOURCESDK_*`, or `PLATFORM_*` list instead of bypassing the module structure.
- Keep platform detection centralized in `cmake/platform/shared.cmake`. Use the repository booleans `WINDOWS`, `LINUX`, and `MACOS` in project CMake modules instead of adding new direct checks in many files.
- Keep platform-specific compiler and linker flags in `cmake/platform/windows.cmake`, `cmake/platform/linux.cmake`, and `cmake/platform/macos.cmake`.
- Root CMake options define the supported feature switches. Prefer extending existing options before introducing a new cache option:
	- `SOURCESDK_GAME_TARGET` selects the game manifest entry from `CMakeGameManifests.json`.
	- `SOURCESDK_AM_DEFINES` enables the manifest `am_defines` block.
	- `SOURCESDK_COMPILE_PROTOBUF` controls whether protobuf is built/generated as part of this project.
	- `SOURCESDK_CONFIGURE_EXPORT_MAP`, `SOURCESDK_LINK_ENABLE_RPATH`, `SOURCESDK_LINK_USE_MOLD`, `SOURCESDK_LINK_STRIP_SYMBOLS`, and `SOURCESDK_LINK_STRIP_CPP_EXPORTS` control Unix link behavior.
	- `SOURCESDK_LINK_TIER0` and `SOURCESDK_LINK_STEAMWORKS` control imported shared libraries.
	- `SOURCESDK_MALLOC_OVERRIDE`, `SOURCESDK_MSVC_RUNTIME_LIBRARY`, and `SOURCESDK_USE_ABI0` affect ABI/runtime compatibility.
- `append_sourcesdk_shared_library( LIB_NAME LIB_FILENAME_OUT IMPLIB_FILENAME_OUT )` resolves imported binary paths under `lib/<platform>/`, returns shared-library and import-library filenames through parent-scope output variables, and may copy/patch Linux shared libraries when `SOURCESDK_LINK_STRIP_CPP_EXPORTS` is enabled. Reuse it for Source SDK imported shared libraries such as `tier0` and `steam_api`.
- `sourcesdk_parse_game_manifests(...)` recursively parses `CMakeGameManifests.json`, follows `inherits`, extracts `name`, `game_dir`, `protobufs_dir`, `defines`, and conditional `am_defines`, then contributes `SE_NAME`, `SE_GAME_DIR`, and game-specific compile definitions. When changing manifests, keep inherited entries minimal and validate configure output for the selected `SOURCESDK_GAME_TARGET`.
- `append_proto_dirs( OUT_ARGS PROTO_DIRS )` converts proto include directories into `-I...` arguments. Pass list variables carefully; this function writes the result to the named output variable in the parent scope.
- `sourcesdk_compile_protos( PROTO_FILENAMES PROTO_ARGS PROTO_DIR PROTO_OUTPUT_DIR LOGS_DIR ERROR_LOGS_DIR PROTO_OUT_PREFIX )` invokes the repository `protoc`, creates output/log/error directories, and skips files whose generated `.pb.cc` already exists. Do not replace it with ad hoc `execute_process()` calls for project protos.
- Protobuf selection is driven by `SOURCESDK_PROTOS`, `SOURCESDK_CUSTOM_PROTOS`, `SOURCESDK_SKIP_PROTOS`, and `SOURCESDK_CUSTOM_SKIP_PROTOS`. Keep entries as proto base names without the `.proto` suffix.
- Generated proto outputs live under `${CMAKE_CURRENT_BINARY_DIR}/protos` by default. Avoid committing generated `.pb.*` files unless a specific task or repository convention requires it.
- `cmake/sourcesdk/proto/clean_prev.cmake` removes stale generated proto headers from `common/` when `SOURCESDK_GAME_TARGET` changes. Be careful when changing this logic because it performs file deletion during configure.
- Target modules under `cmake/sourcesdk/targets/` create either static project libraries or imported shared libraries and expose aliases as `${PROJECT_NAME}::<target>`. Follow the existing target-property block for C/C++ standards, MSVC runtime, macOS architecture, compile options, definitions, includes, and link libraries.
- Keep binary compatibility flags deliberate. Do not casually change `_GLIBCXX_USE_CXX11_ABI`, exported symbol maps, RPATH, MSVC runtime selection, SSE flags, or `_WIN32`/`POSIX` compile definitions.

## Build and Verification

- Prefer the narrowest useful verification for the change:
	- Header-only or formatting changes: run formatting or a targeted compile check if available.
	- C++ implementation changes: build the smallest relevant target or run the existing test/compile command used by the project.
	- CMake/tooling changes: run CMake configure or the relevant preset when practical.
	- Gamedata changes: validate JSON syntax and, when possible, verify signatures/offsets against the binary evidence.
- Use `rg` for searching. Use `rg --files` for file discovery.
- Do not run expensive full builds unless the task's risk justifies it or the user asks for it.
- If verification cannot be run locally, say exactly what was not run and why.

## Git and Workspace Safety

- The worktree may contain user changes. Do not reset, checkout, delete, or rewrite files you did not intentionally modify.
- Inspect `git status --short` before and after edits when making repository changes.
- Keep commits, branch operations, rebases, and force updates out of scope unless the user explicitly requests them.
- Do not modify generated files unless the task requires it or the repository expects generated outputs to be checked in.

## Commit Message Style

- Match the existing short imperative commit style. Use a capitalized verb first: `Add`, `Update`, `Remove`, `Fix`, `Correct`, `Move`, or `Actualize`.
- Keep the subject concise and technical. Prefer one line unless the user asks for a detailed commit body.
- Put C++ symbols, interfaces, classes, methods, and important file-like identifiers in backticks:

```text
Add `CFieldPath` class
Update `CEntityIndex` & `CPlayerSlot`
Fix missing `AddRef` in `CSmartPtr::CopyFrom`
```

- For CMake-only changes, a prefix such as `CMake:` is acceptable:

```text
CMake: add `cs2-beta` game
```

- Avoid vague lowercase subjects such as `build fix`, `fix: ...`, or typoed verbs in new commits even if older history contains examples.
- When adding an AI co-author, use a standard trailer after a blank line:

```text
Co-authored-by: Codex <codex@openai.com>
```

## Documentation

- Write technical documentation in clear English.
- Prefer direct, operational guidance over broad style advice.
- Keep examples short and matching the project's C++ style.
- Update nearby documentation when behavior, public APIs, build flags, or gamedata formats change.

## Context Window Management

- When the context window becomes large or the session is about to be compacted, produce a concise technical summary that preserves the current task, repository state, files changed, commands run, verification status, open risks, and the exact next steps.
- The compacted context must preserve and continue to enforce this `AGENTS.md` file. After compaction, keep following these rules strictly instead of restarting from generic defaults.
- Do not omit important constraints from this file when summarizing context, especially code style, CMake conventions, `ida-pro-mcp` usage for reverse-engineering work, and workspace safety rules.
