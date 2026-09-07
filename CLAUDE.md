# Source SDK Agent Instructions

Valve/Source SDK C++ with AlliedModders-specific modernizations. Keep edits local, conservative, and consistent with the surrounding file. Never reformat unrelated code.

This file is the Claude Code adaptation of [AGENTS.md](AGENTS.md), which is the upstream-owned source of these conventions. When upstream updates `AGENTS.md`, re-sync this file.

## Repository Orientation

- Treat this repository root as the working directory unless the task explicitly points elsewhere.
- Prefer existing project helpers, naming conventions, container types, allocation patterns, and platform abstractions over new generic utilities.
- Source is mainly C and C++. Tooling is CMake, JSON manifests, and platform-specific SDK directories.
- Library code lives in [common/](common/), [entity2/](entity2/), [game/](game/), [interfaces/](interfaces/), [mathlib/](mathlib/), [networksystem/](networksystem/), [public/](public/), [tier1/](tier1/). [tests/](tests/) holds the CTest suite, [sym/](sym/) the export maps, [lib/](lib/) the imported binaries (`tier0`, `steam_api`), [devtools/bin/](devtools/bin/) the per-platform `protoc`. [thirdparty/](thirdparty/) (`protobuf`, `game_protobufs`) is vendored/submodule — leave it alone unless the task is explicitly about it.
- Project targets build as C17 / C++17; [tests/](tests/) requires C++23. Use what the module you are editing already has available.
- Preserve compatibility with existing Source SDK code. Do not introduce modern C++ facilities into old subsystems unless nearby code already uses them or the task requires it.
- Do not sort includes unless the touched file already uses sorted include groups — [.clang-format](.clang-format) sets `SortIncludes: false` intentionally.

## Tooling

- Use Read / Grep / Glob for reading and searching. Do not shell out to `cat`, `grep`, `rg`, `find`, `sed`, or `head`.
- Use Edit for partial changes; Write only for new files or a full replacement of a file already read.
- Bash/PowerShell are for git, cmake, ninja, and other real commands — not file inspection.
- When the `codebase-memory-mcp` server is active, `search_graph` / `trace_path` / `get_code_snippet` locate symbols across this header tree faster than a broad Grep. Confirm every hit against the file before relying on it.

## Formatting

Follow [.clang-format](.clang-format) first, then the local file style.

- Tabs for indentation. Tab width and indent width are 4.
- Allman braces:

```cpp
if ( bEnabled )
{
	DoWork();
}
```

- Spaces inside parentheses for declarations, calls, expressions, and control statements:

```cpp
Foo( nArgCount );
if ( bCondition )
if constexpr ( COMPILER_CONDITION )
while ( true )
```

- Spaces inside angle brackets for templates:

```cpp
template < size_t SIZE >
CBufferStringN< SIZE >
```

- Spaces inside square brackets around an array's element count. Empty brackets stay tight:

```cpp
int m_nChild[ 2 ];
char m_pGameInfoPath[ MAX_PATH ];
int nValues[] = { 1, 2, 3 };
```

- Same visual spacing for explicit casts in edited code:

```cpp
( int )( nValue )
static_cast< int >( nValue )
```

- Spaces inside braced initializer lists and short macro bodies containing values or statements:

```cpp
int nValues[] = { 1, 2, 3 };
#define Assert_BSO( exp ) { if ( IsStackAllocated() ) Assert( exp ); }
```

- No space before `(` for function declarations, definitions, and calls; a space before `(` for control statements:

```cpp
void SetValue( int nValue );
SetValue( nValue );

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

- Keep short inline functions on one line only when that matches surrounding code and stays readable:

```cpp
int Length() const { return m_nLength; }
```

- `ColumnLimit: 0` — keep long argument, template, and call lists on one line when the surrounding code does. Do not wrap purely because a line is long.
- Avoid alignment-only churn. Do not realign unrelated declarations, assignments, comments, or tables.
- Keep the existing line endings and encoding of the file being edited.

## Naming

- Preserve Source-style prefixes and Hungarian-like conventions:
	- `p` pointers (`pString`, `pData`), `n` integer counts/sizes (`nLen`, `nCount`), `b` booleans (`bAllowHeapAllocation`), `m_` members (`m_nLength`).
	- `C` for classes, `I` for interfaces, `_t` suffix for many enum/struct typedef-style names.
	- `M` prefix for compile-time metaprogramming structures and traits.
- Do not rename symbols to modernize them. Keep public ABI/API names stable unless the task is explicitly a rename.

## C++ Practices

- Prefer existing project types and APIs — `CBufferString`, `CUtlString`, `CUtlBuffer`, `CUtlVector`, `CUtlLeanVector`, `Q_*` / `V_*` string helpers, `Assert`, `Move`, platform abstraction headers — when nearby code uses them.
- Be careful with ownership and allocation. Many modules have custom allocation, fixed-buffer, stack-buffer, or platform-specific lifetime expectations.
- Preserve binary layout, vtable layout, exported symbols, calling conventions, and public header contracts.
- Avoid exceptions and RTTI-dependent designs unless the surrounding subsystem already uses them.
- Keep platform guards precise. Do not widen `_WIN32`, POSIX, Xbox, PS3, or dedicated-server paths casually.
- When adding overloads or templates to public headers, weigh compile-time cost and ABI/API ambiguity.
- Comments explain non-obvious constraints, compatibility requirements, reverse-engineered behavior, or ownership rules. Never restate the code.

## Reverse Engineering and Binary Work

For anything involving signatures, offsets, vtables, calling conventions, binary compatibility, gamedata, disassembly, or behavior reconstructed from a binary:

- The `ida-pro-mcp` MCP server is the primary source for IDA-derived facts. Its tool schemas are deferred — load them with ToolSearch (`select:mcp__ida-pro-mcp__list_instances,mcp__ida-pro-mcp__decompile,...`) before calling.
- One IDA instance is open per module. Call `list_instances` first, then `select_instance`; `server_health` confirms which binary an instance actually has loaded. The instance→binary mapping shifts between game builds — never carry it over from a previous session.
- IDAPython runs through the same server: `py_eval` for expressions, `py_exec_file` for scripts.
- Prefer direct evidence — functions, xrefs, names, types, strings, decompiler output — over memory or inference.
- Never invent addresses, offsets, signatures, symbol names, or vtable indexes. If IDA is unavailable, say so explicitly and use repository evidence only.
- Record important IDA-derived assumptions in the final response or a nearby technical comment when the code would otherwise be hard to justify.
- For gamedata/signature changes, verify game, engine branch, platform, and binary version. Keep platform-specific entries separated; do not broaden a signature without evidence.
- Every binary-reconstructed structure or class with a known size gets `COMPILE_TIME_ASSERT( sizeof( TypeName ) == 0xSize );` near the declaration that owns the layout. Use the size verified from the target binary, and guard platform- or branch-specific sizes with the matching preprocessor conditions.
- Use string references as primary orientation when reconstructing fields: nearby literals, xrefs, logging/assert messages, schema names, RTTI/typeinfo, constructor/destructor references. Do not infer a semantic name from offset alone when string evidence exists.
- Game modules are stripped, but the schema system embeds Itanium-mangled `typeid( T ).name()` strings; `find_regex` over strings recovers exact template signatures. A `_ZTS` string with no matching `_ZTI` object means the class is not polymorphic — do not give it a vtable.
- Preserve exact field offsets and padding. Use explicit padding members only when the real field type or purpose is unknown, and name them so the offset range is clear.
- Preserve exact vtable slot order. Never reorder, remove, or collapse virtual methods because their purpose is unknown — if a slot exists in the binary, the SDK declaration keeps a corresponding slot.
- Mark unknown virtuals as `Unk_IntendedMethodName( void *p )` when a plausible name is known but the signature is not. Keep the placeholder in its exact slot and prefer a single opaque `void *p` until the signature is verified.
- If neither name nor signature is known, use a slot-preserving name carrying the vtable index or offset; rename only once binary evidence supports the real meaning.
- When a signature is partially known, do not "improve" it with guessed argument types. Keep opaque placeholders and document what evidence would replace them.

Keep comments on reconstructed code down to what the reader cannot get from tooling:

- Do not annotate members with their offsets. clangd in VS Code and CLion already show the offset on hover, so a `// 0x38` comment adds nothing and goes stale as soon as the layout shifts.
- Do not name the library or module a reconstruction came from. That belongs in the reply or the commit message, not in the header.
- Do not point at other headers by filename. Describe what a declaration is and how it behaves; where it lives is the include graph's job, and the reference rots when files move.
- Do not add `AMNOTE:` markers. That marker is for repositories under the AlliedModders LLC organization; this fork does not introduce new ones.

## CMake Conventions

- Project-owned CMake lives under [cmake/](cmake/) and [CMakeLists.txt](CMakeLists.txt). `thirdparty/protobuf/**` CMake is vendored — do not touch it unless the task is explicitly about vendored protobuf.
- Style: lowercase built-in commands, uppercase project variables, tabs, multi-line `set()` / `list(APPEND ...)`:

```cmake
set(SOURCESDK_TIER1_SOURCE_FILES
	${SOURCESDK_TIER1_DIR}/bitbuf.cpp
	${SOURCESDK_TIER1_DIR}/tier1.cpp
)
```

- Preserve the aggregate-list pattern. `SOURCESDK_SOURCE_FILES`, `SOURCESDK_INCLUDE_DIRS`, `SOURCESDK_COMPILE_DEFINITIONS`, `SOURCESDK_LINK_LIBRARIES`, `PLATFORM_COMPILE_OPTIONS`, `PLATFORM_LINK_OPTIONS`, and `PLATFORM_COMPILE_DEFINITIONS` are intentionally accumulated across included modules.
- When adding sources, include dirs, definitions, link options, or libraries, append to the nearest existing `SOURCESDK_<MODULE>_*`, `SOURCESDK_*`, or `PLATFORM_*` list instead of bypassing the module structure.
- Keep platform detection centralized in [cmake/platform/shared.cmake](cmake/platform/shared.cmake). Use the repository booleans `WINDOWS`, `LINUX`, `MACOS` rather than adding new direct checks. Platform-specific compiler/linker flags belong in [cmake/platform/windows.cmake](cmake/platform/windows.cmake), [linux.cmake](cmake/platform/linux.cmake), [macos.cmake](cmake/platform/macos.cmake).
- Extend existing root options before adding a new cache option:
	- `SOURCESDK_GAME_TARGET` selects the manifest entry from [CMakeGameManifests.json](CMakeGameManifests.json) (default `cs2`).
	- `SOURCESDK_AM_DEFINES` enables the manifest `am_defines` block.
	- `SOURCESDK_COMPILE_PROTOBUF` controls whether protobuf is built/generated here.
	- `SOURCESDK_CREATE_INTEFACE_OVERRIDE` (spelling as-is) for a custom `CreateInterface`; `SOURCESDK_ENABLE_TESTS` builds [tests/](tests/).
	- `SOURCESDK_CONFIGURE_EXPORT_MAP`, `SOURCESDK_LINK_ENABLE_RPATH`, `SOURCESDK_LINK_USE_MOLD`, `SOURCESDK_LINK_STRIP_SYMBOLS`, `SOURCESDK_LINK_STRIP_CPP_EXPORTS` control Unix link behavior.
	- `SOURCESDK_LINK_TIER0`, `SOURCESDK_LINK_STEAMWORKS` control imported shared libraries.
	- `SOURCESDK_GENERATE_CLANGD` writes `.clangd` at the SDK root with the `sourcesdk` target flags, so clangd resolves SDK headers that have no `compile_commands.json` entry.
	- `SOURCESDK_MALLOC_OVERRIDE`, `SOURCESDK_MSVC_RUNTIME_LIBRARY`, `SOURCESDK_USE_ABI0` affect ABI/runtime compatibility.
- `append_sourcesdk_shared_library( LIB_NAME LIB_FILENAME_OUT IMPLIB_FILENAME_OUT )` resolves imported binary paths under `lib/<platform>/`, returns shared-library and import-library filenames through parent-scope outputs, and may copy/patch Linux shared libraries when `SOURCESDK_LINK_STRIP_CPP_EXPORTS` is on. Reuse it for imported Source SDK shared libraries such as `tier0` and `steam_api`.
- `sourcesdk_generate_clangd( TARGET <target> [OUTPUT <file>] [PATH_MATCH <regex>...] )` in [cmake/sourcesdk/clangd.cmake](cmake/sourcesdk/clangd.cmake) writes a `.clangd` fragment with the target's include directories, definitions, compile options, and C++ standard flag via generator expressions. `PATH_MATCH` regexes are relative to the output file's directory. Projects that `add_subdirectory()` this repository call it for their own targets (e.g. headers of a single-TU plugin).
- `sourcesdk_parse_game_manifests(...)` recursively parses `CMakeGameManifests.json`, follows `inherits`, extracts `name`, `game_dir`, `protobufs_dir`, `defines`, conditional `am_defines`, and contributes `SE_NAME`, `SE_GAME_DIR`, and game-specific definitions. Keep inherited entries minimal and validate configure output for the selected `SOURCESDK_GAME_TARGET`.
- `append_proto_dirs( OUT_ARGS PROTO_DIRS )` converts proto include dirs into `-I...` arguments and writes to the named parent-scope variable — pass list variables carefully.
- `sourcesdk_compile_protos( PROTO_FILENAMES PROTO_ARGS PROTO_DIR PROTO_OUTPUT_DIR LOGS_DIR ERROR_LOGS_DIR PROTO_OUT_PREFIX )` invokes the repository `protoc`, creates output/log/error dirs, and skips files whose `.pb.cc` already exists. Do not replace it with ad hoc `execute_process()`.
- Protobuf selection is driven by `SOURCESDK_PROTOS`, `SOURCESDK_CUSTOM_PROTOS`, `SOURCESDK_SKIP_PROTOS`, `SOURCESDK_CUSTOM_SKIP_PROTOS` — entries are base names without `.proto`. Generated outputs live under `${CMAKE_CURRENT_BINARY_DIR}/protos`; do not commit generated `.pb.*` unless the task requires it.
- [cmake/sourcesdk/proto/clean_prev.cmake](cmake/sourcesdk/proto/clean_prev.cmake) deletes stale generated proto headers from `common/` when `SOURCESDK_GAME_TARGET` changes. It performs file deletion during configure — change it carefully.
- Target modules under [cmake/sourcesdk/targets/](cmake/sourcesdk/targets/) create static project libraries or imported shared libraries and expose `${PROJECT_NAME}::<target>` aliases. Follow the existing target-property block for C/C++ standards, MSVC runtime, macOS architecture, compile options, definitions, includes, and link libraries.
- Keep binary-compatibility flags deliberate. Do not casually change `_GLIBCXX_USE_CXX11_ABI`, export maps, RPATH, MSVC runtime selection, SSE flags, or `_WIN32`/`POSIX` definitions.

## Build and Verification

Pick the narrowest useful verification:

| Change | Verification |
| --- | --- |
| Header-only / formatting | formatting check or a targeted compile |
| C++ implementation | build the smallest relevant target, or the existing test/compile command |
| CMake / tooling | `cmake --preset <name>` configure, or the relevant preset build |
| Gamedata | validate JSON syntax; verify signatures/offsets against binary evidence |

- Configure presets in [CMakePresets.json](CMakePresets.json): `VisualStudio` (Windows only) and the Ninja configs `Debug`, `RelWithDebInfo`, `Release`. All of them force `SOURCESDK_ENABLE_TESTS` on; the root option itself defaults to `OFF`. Build dirs are `build/<hostSystemName>/<presetName>`.
- Build and test presets carry the same names, plus `VisualStudio\Debug` and `VisualStudio\Release`. Test presets run CTest with `outputOnFailure`.
- [tests/](tests/) uses the in-repo runner under `tests/common/`, not an external framework — one executable per source file, registered as `<name>_tests` or `smoke_<name>_tests`. Narrow a run with `ctest --preset Debug -R utlvector`.
- CI ([.github/workflows/](.github/workflows/)) configures and builds `Debug`, `RelWithDebInfo`, and `Release` on Linux, macOS, and Windows, and runs `ctest` on the Debug preset only.
- Do not run expensive full builds unless the risk justifies it or the user asks.
- If verification could not be run, state exactly what was not run and why. Never report a build or test as passing that you did not observe pass.

## Git and Workspace Safety

- The worktree may contain user changes. Do not reset, checkout, delete, or rewrite files you did not intentionally modify.
- Check `git status --short` before and after repository edits.
- Commits, branch operations, rebases, and force updates are out of scope unless the user explicitly requests them.
- Do not modify generated files unless the task requires it.

## Commit Message Style

Match the style already dominant on the branch you are committing to.

- **This fork's own history** uses conventional-commit prefixes: `fix: correct size check for CWorldRendererMgr`, `feat: add CVariableBitString`.
- **Upstream style**, documented in `AGENTS.md`, is short imperative with a capitalized verb — `Add`, `Update`, `Remove`, `Fix`, `Correct`, `Move`, `Actualize`. Use it for commits intended to go upstream.

Either way, keep the subject concise and technical, one line unless a body is requested, and put C++ symbols, interfaces, classes, methods, and file-like identifiers in backticks:

```text
Add `CFieldPath` class
Update `CEntityIndex` & `CPlayerSlot`
Fix missing `AddRef` in `CSmartPtr::CopyFrom`
```

For CMake-only changes a `CMake:` prefix is acceptable:

```text
CMake: add `cs2-beta` game
```

Avoid vague subjects such as `build fix` or typoed verbs, even where older history contains them. When adding an AI co-author, use a trailer after a blank line:

```text
Co-Authored-By: Claude Opus 5 (1M context) <noreply@anthropic.com>
```

## Documentation

- Write technical documentation in clear English.
- Prefer direct, operational guidance over broad style advice.
- Keep examples short and matching the project's C++ style.
- Update nearby documentation when behavior, public APIs, build flags, or gamedata formats change.

## Context Compaction

When the session is compacted, the summary must preserve the current task, repository state, files changed, commands run, verification status, open risks, and exact next steps.

This `CLAUDE.md` stays in force across compaction. After compaction, keep following it strictly rather than reverting to generic defaults — especially the code style, CMake conventions, `ida-pro-mcp` usage for reverse-engineering work, and workspace safety rules.
