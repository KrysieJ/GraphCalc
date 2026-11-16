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
msbuild "Legacy Code\Win32\GrphCalc.vcxproj" /p:Configuration=Debug /p:Platform=x64 /m /fl /flp:logfile=build.log;verbosity=normal
```

Work through the errors in small batches. After each batch, rebuild and check the error count went down.

## Rules for changes

- Keep fixes minimal and behaviour-preserving. This is a port, not a rewrite. No refactoring, renaming or reformatting.
- Source files use CRLF line endings and Latin-1/ASCII encoding. Preserve both.
- Do not delete files. Do not edit the `.dsp`, `.dsw`, CVS folders or other VC6 artifacts.
- Do not commit or push unless asked.
- Record every change that is not an obvious compile fix in `PORTING.md` (what, why, where).
- If a fix would change program behaviour, or contradicts `PORTING.md`, stop and ask instead of guessing.

## Working with the owner

- The owner is a network engineer, not a programmer. Explain what a change does and why in plain terms; do not expect them to read C++.
- Security and accuracy matter more than speed. Say when something is unverified.
- Cite sources for claims about toolchains, APIs or platform behaviour.
- Written documents use IEEE (2006) reference formatting.
