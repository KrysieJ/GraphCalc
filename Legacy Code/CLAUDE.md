# Legacy Code: rules for "dirty-side" sessions only

Everything in this folder is the original GraphCalc C++ (GPL-2.0-only), patched
in Steps 1 and 2. **A session that reads anything in this folder is a dirty-side
session for the rest of its life.** It may write behaviour specifications in
`spec/`, capture golden results and maintain the legacy build. It must never
write or edit anything under `rust/`. See "Clean-room rule" in the root `CLAUDE.md`.

## Where things are

- `Win32/` - the original MFC sources, patched for Steps 1 and 2.
- `Win32/GrphCalc.vcxproj` - the build project (v145 toolset, Visual Studio 2026, MFC shared DLL, MBCS, C++14, Spectre mitigation on).
- `Win32/PORTING.md` - every Step 1 and Step 2 change, the compromises, the warnings log and the smoke tests. Read it before changing legacy code.

## Building the legacy program

Run from a Developer PowerShell for Visual Studio 2026 (version 18), from the repository root:

```
msbuild "Legacy Code\Win32\GrphCalc.vcxproj" /p:Configuration=Debug /p:Platform=x64 /m /fl "/flp:logfile=build.log;verbosity=normal"
```

Change only `Configuration` and `Platform`. Output goes to `Legacy Code\Win32\x64\<Configuration>\`.

## Rules for changing legacy code

- Keep fixes minimal and behaviour-preserving. No refactoring, renaming or reformatting.
- Source files use CRLF line endings. Most are plain ASCII; files that contain non-ASCII characters are UTF-8 with a byte-order mark (see `PORTING.md`). Preserve both. Do not introduce non-ASCII characters into string literals or resource files without asking, because the program is built as multi-byte (MBCS), not Unicode.
- Do not delete files. Do not edit the `.dsp`, `.dsw`, CVS folders or other VC6 artifacts.
- Record every change that is not an obvious compile fix in `PORTING.md` (what, why, where), and keep the logs at the bottom of it up to date.
- If a fix would change program behaviour, or contradicts `PORTING.md`, stop and ask.
- Default effort is medium; ask the owner for `/effort high` if a problem needs it.

## Rules for writing specifications (`spec/`)

- Describe **behaviour only**: what the user types or does, what the program shows, error messages, rounding and formatting rules, limits, file formats.
- Never copy source code, comments, internal variable, function or class names, or the structure of the code (how it is split into functions, the order of steps inside an algorithm unless that order changes the visible result).
- User-visible names are fine: the functions and operators a user types (`sin`, `nCr`), script command names (`2DXmin`), menu items, dialog titles, error message text.
- Where the behaviour is surprising or looks like a bug, describe it and mark it `QUIRK:` so the owner can decide whether the port keeps it.
- Each spec file starts with `Spec-Author: dirty-side session, <date>` and `Reviewed: no` until the owner or the reviewing chat marks it reviewed.
