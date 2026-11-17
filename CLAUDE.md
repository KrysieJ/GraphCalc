# GraphCalc Rebuild

Reviving GraphCalc (GPL, 1999-2003, Visual C++ 6.0 / MFC) in four steps:

1. Scrub the legacy code so it compiles with a current toolchain. (Done by static analysis; not yet compiled.)
2. Build a working Windows 11 x64 version from the updated code. **(Current step.)**
3. Port to Rust and make it publishable in the Microsoft Store.
4. Design additional features.

## Where things are

- `Legacy Code/Win32/` - the original MFC sources, already patched for Step 1.
- `Legacy Code/Win32/GrphCalc.vcxproj` - the build project (v145 toolset, Visual Studio 2026 Insiders, MFC shared DLL, MBCS, C++14).
- `Legacy Code/Win32/PORTING.md` - **read this first.** Every Step 1 change, the deliberate compromises and the list of things expected to break on first compile.
- `GraphCalc/` - an empty UWP template project. Not part of Step 2; do not build it.

## Building (Step 2)

Run from a Developer PowerShell for Visual Studio 2026 (version 18) so `msbuild` is on the path:

```
msbuild "Legacy Code\Win32\GrphCalc.vcxproj" /p:Configuration=Debug /p:Platform=x64 /m /fl "/flp:logfile=build.log;verbosity=normal"
```

Work through the errors in small batches. After each batch, rebuild and check the error count went down.

This work may run unattended across usage-limit resets. Keep a short running log at the bottom of `PORTING.md` (date, error count before and after, what was fixed) so progress survives a restart.

Default effort is medium. If a problem genuinely needs deeper reasoning (for example a design decision, an MFC behaviour question, or the same error surviving two fix attempts), stop and ask the owner to switch to `/effort high` rather than guessing.

## Shell commands (keep permission prompts down)

The owner approves commands by hand, and PowerShell commands containing subexpressions prompt every time, even when an allow rule covers them. So:

- Run one plain command per tool call. Do not chain with `;`, `&&`, `|` or wrap in `$(...)`, `@(...)`, `{...}` script blocks or `ForEach-Object`.
- Run the build with exactly the command shown above, unchanged, so a single "don't ask again" approval keeps covering it.
- To inspect `build.log` or source files, use the Read and Grep tools, not `Select-String`, `Get-Content` pipelines or other PowerShell one-liners.
- Use the Edit tool for file changes, never PowerShell or `sed`.

## Rules for changes

- Keep fixes minimal and behaviour-preserving. This is a port, not a rewrite. No refactoring, renaming or reformatting.
- Source files use CRLF line endings. Most are plain ASCII; files that contain non-ASCII characters are UTF-8 with a byte-order mark (converted from Latin-1 in Step 2, see `PORTING.md`). Preserve both. Do not introduce non-ASCII characters into string literals or resource files without asking, because the program is built as multi-byte (MBCS), not Unicode.
- Do not delete files. Do not edit the `.dsp`, `.dsw`, CVS folders or other VC6 artifacts.
- Do not commit or push unless asked.
- Record every change that is not an obvious compile fix in `PORTING.md` (what, why, where).
- If a fix would change program behaviour, or contradicts `PORTING.md`, stop and ask instead of guessing.

## Working with the owner

- The owner is a network engineer, not a programmer. Explain what a change does and why in plain terms; do not expect them to read C++.
- Security and accuracy matter more than speed. Say when something is unverified.
- Cite sources for claims about toolchains, APIs or platform behaviour.
- Written documents use IEEE (2006) reference formatting.
