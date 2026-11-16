# GraphCalc legacy source: porting notes (Step 1)

Date: 16 Nov. 2026
Scope: `Legacy Code/Win32` (Visual C++ 6.0 / MFC 6, last touched 2003)
Target: Visual Studio 2022 (v143 toolset), MFC, Windows 11 x64

## Status: patched, NOT yet compiled

Every change below was made by static analysis on a machine without the
Microsoft compiler or MFC headers. Nothing here has been through `cl.exe`.
Expect the first real build to surface further errors; section 5 lists where
they are most likely. Treat this as "known breakages removed", not "builds clean".

## 1. How to build

1. Visual Studio Installer: workload **Desktop development with C++**, plus the
   individual component **C++ MFC for latest v143 build tools (x86 & x64)**.
2. Open `GraphCalc Legacy.sln`. The new project is **GrphCalc**
   (`Legacy Code\Win32\GrphCalc.vcxproj`). Right-click it and choose
   *Set as Startup Project*, select **Debug | x64**, then build *the project*
   (not the solution; the solution also holds the empty UWP template project).
3. Output goes to `x64\Debug\GrphCalc.exe` at the repository root. A post-build
   step copies `constants.dat`, `units.dat`, `default.gcl`, `graphcalc.ini`,
   `grphcalc.chm` and `scripts\` beside the executable, because the program
   treats its start-up working directory as its application directory.

## 2. Source changes (60 files; review with `git diff`)

| # | Change | Why | Where |
|---|--------|-----|-------|
| 1 | `<fstream.h>` and the global `::ifstream` / `::ofstream` / `::fstream` replaced with `<fstream>` and `std::` types; `ios::nocreate`, `filebuf::openprot`, `filebuf::sh_none` removed or mapped | The pre-standard iostream library no longer ships; the old headers must be replaced [1], [2] | `CCalculator.*`, `GrphCalcDlg.*`, `OutputView.cpp`, `OptionsDlg.cpp`, `SaveScriptDlg.cpp`, `2DTableViewDlg.cpp`, `3DGraph.cpp`, `graphview.cpp`, `Keywords.h` |
| 2 | New `GCGetLine()` helper in `globals.h`; all 31 `stream.getline(buf, n, '\n')` calls routed through it | Behaviour preservation. Standard `getline` sets `failbit` when a line is longer than the buffer; the old library did not. Without the wrapper every `while (!file.eof())` loop could spin forever on a long line | `globals.h` + the files in row 1 |
| 3 | Loop variables that were used after their `for` loop are now declared just before the loop (12 places) | Standard `for` scoping; the old behaviour is only available through a deprecated switch [3], [4] | `2DInfoBar.h`, `3DGraph.cpp`, `CCalculator.cpp`, `GrphCalcDlg.cpp`, `Menus.h`, `graphview.cpp` |
| 4 | `ON_MESSAGE` handlers changed to `LRESULT fn(WPARAM, LPARAM)`; two `void` handlers now `return 0` | Required handler signature [5]; the old `LONG (UINT, LONG)` form is also wrong-sized on x64 | `ColourPicker.*`, `3DGORenderMode.*`, `CalculatorView.cpp`, `GrphCalcDlg.h`, `Menus.h`, `graphview.*` |
| 5 | `OnTimer(UINT)` to `OnTimer(UINT_PTR)` (5 classes); `OnActivateApp(BOOL, HTASK)` to `(BOOL, DWORD)` | Current MFC handler prototypes | `2DNSliderDlg`, `3DGraph`, `CJComboBox`, `FlatEdit`, `Tfxdatatip`, `ColourPopup` |
| 6 | Every message-map entry now names its handler as `&Class::Handler` (451 entries) | Standard pointer-to-member syntax; lets the project move to `/permissive-` later | all files with a `BEGIN_MESSAGE_MAP` |
| 7 | 64-bit casts: `(UINT)hMenu` to `(UINT_PTR)` (13), rich-edit stream cookie `DWORD` to `DWORD_PTR`, `ShellExecute`/`WinExec` result casts, tooltip `idFrom` | These truncated pointers/handles on x64 | `3DGraph.cpp`, `ContextMenus.h`, `GrphCalcDlg.cpp`, `OutputView.cpp`, `graphview.cpp`, `HyperLink.cpp` |
| 8 | `HtmlHelp(...)` to `::HtmlHelp(...)` inside the dialog class | `CWnd` now has its own 2-argument `HtmlHelp` member that hides the Win32 function | `GrphCalcDlg.cpp` |
| 9 | `CFindAreaDlg::PassEq` given an explicit `void` return type | Implicit `int` is no longer accepted [4] | `FindAreaDlg.*` |
| 10 | `#include <GLAux.h>` removed; `glaux.lib` dropped from the link line | GLAUX is not in the current Windows SDK; the code never called it | `StdAfx.h`, project |
| 11 | `Enable3dControls()` / `Enable3dControlsStatic()` calls removed | Obsolete no-ops | `GrphCalc.cpp` |
| 12 | `delete p` to `delete[] p` for 17 `new char[n]` buffers | Mismatched array delete (undefined behaviour); no functional change | `CCalculator.cpp`, `GrphCalcDlg.cpp`, `OutputView.cpp` |
| 13 | `#include <SDKDDKVer.h>` added ahead of the MFC headers | Declares the target Windows version | `StdAfx.h` |

## 3. Project and repository changes

* **New** `GrphCalc.vcxproj` / `.filters`, generated from the file list in
  `GrphCalc.dsp`: 63 sources, shared MFC, multi-byte character set, C++14,
  Debug/Release for x64 and Win32, same nine files excluded from the
  precompiled header as in the original. MBCS is a supported configuration
  again as of Visual Studio 2017 [6].
* `GraphCalc Legacy.sln`: GrphCalc project added. The UWP template project is untouched.
* `.gitignore`: the stock rule `[Ww][Ii][Nn]32/` matched the source folder
  itself, so **any new file under `Legacy Code/Win32` was silently ignored by
  git**. A re-include rule was appended. Build output there is still ignored.
* Left out of the project, as in the `.dsp`: `Copy of 3DGraph.cpp`, `CDebug`,
  `colorbtn`, `DlgModalToModeless`, `InsertConstantDlg`, `InsertUnitDlg`,
  `NewUnitCategoryDlg`, `ProgramOptions`, `ProgressWnd`, `Read.cpp`,
  `RegisterDlg`, `ShadeButton`. Not patched. Nothing was deleted (CVS folders,
  `.scc`, `.aps`, `.opt`, `.plg`, `.clw` are still present).
* The "GraphCalc Light" configurations (`GRAPHCALC_LIGHT`) were not recreated.

## 4. Deliberate compromises in the project settings

| Setting | Value | Reason | Revisit |
|---------|-------|--------|---------|
| Conformance mode | off (`/permissive`) | VC6-era code assigns string literals to `char*` and similar | After first clean build |
| SDL checks | off | They turn deprecated-CRT warnings into errors | With the CRT clean-up below |
| `_CRT_SECURE_NO_WARNINGS`, `_CRT_NONSTDC_NO_WARNINGS` | defined | 26 calls to `strcpy`, `sscanf`, `lstrcat`, `chdir`, `WinExec` and friends | Replace calls, then remove |

## 5. Not verified / likely next errors

* Anything MFC changed that static inspection cannot see: virtual-function
  signature drift, `CString` template ambiguities, `INT_PTR` return types.
  A scripted check of all message-map handlers against current prototypes
  found no remaining mismatches, but that check is heuristic.
* x64 warnings (C4244, C4267, C4311, C4312) should be read once the build
  runs; only the truncations found by search were fixed.
* Runtime dependencies assumed present on Windows 11 but not tested:
  RichEdit 1.0 (`AfxInitRichEdit`, `RICHED32.DLL`), `opengl32`/`glu32`, HTML Help.
* `std::fstream(name, mode, _SH_DENYRW)` uses a Microsoft-specific third
  constructor argument to keep the old exclusive-share behaviour.

## 6. Pre-existing issues noticed, not changed

* Hard-coded root paths: `C:\graphcalc.ini`, `C:\GCtester.txt`,
  `C:\GC Test Results.txt` (`GrphCalcDlg.cpp`, `OutputView.cpp`).
* Fixed-size line buffers (50 to 2048 bytes) for all file parsing; safe from
  overflow because `getline` is bounded, but long lines are split.
* `HyperLink.cpp` falls back to `WinExec` with a command line built from the registry.
* `WM_2DGRAPH_N_SLIDER_UPDATE` and `WM_2DGRAPH_N_SLIDER_CLOSING` are both `WM_USER + 5` (`globals.h`).
* `globals.h` redefines `NULL`.
* The program writes history, settings and a file named `temp` into its working directory.

## References

[1] Microsoft, "Differences in iostream implementation," Visual Studio 6.0 documentation. [Online]. Available: https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa293990(v=vs.60) (accessed Nov. 16, 2026).

[2] Microsoft, "Frequently asked questions about the Standard C++ library." [Online]. Available: https://learn.microsoft.com/en-US/troubleshoot/cpp/faq-standard-cpp-library (accessed Nov. 16, 2026).

[3] Microsoft, "Visual C++ what's new 2003 through 2015." [Online]. Available: https://learn.microsoft.com/en-us/cpp/porting/visual-cpp-what-s-new-2003-through-2015 (accessed Nov. 16, 2026).

[4] Microsoft, "Migrating from VC6," TexBlog archive. [Online]. Available: https://learn.microsoft.com/en-us/archive/blogs/texblog/migrating-from-vc6 (accessed Nov. 16, 2026).

[5] Microsoft, "User-defined handlers," MFC reference. [Online]. Available: https://learn.microsoft.com/en-us/cpp/mfc/reference/user-defined-handlers (accessed Nov. 16, 2026).

[6] Microsoft, "Unicode and multibyte character set (MBCS) support." [Online]. Available: https://learn.microsoft.com/en-us/cpp/atl-mfc-shared/unicode-and-multibyte-character-set-mbcs-support (accessed Nov. 16, 2026).

Rows 5, 7, 8, 10 and 11 in section 2 rest on knowledge of current MFC and the
Windows SDK rather than a cited page; the first build will confirm or refute them.
