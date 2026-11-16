# GraphCalc legacy source: porting notes (Step 1)

Date: 16 Nov. 2026
Scope: `Legacy Code/Win32` (Visual C++ 6.0 / MFC 6, last touched 2003)
Target: Visual Studio 2026 (version 18, v145 toolset), MFC, Windows 11 x64

## Status: patched, NOT yet compiled

Every change below was made by static analysis on a machine without the
Microsoft compiler or MFC headers. Nothing here has been through `cl.exe`.
Expect the first real build to surface further errors; section 5 lists where
they are most likely. Treat this as "known breakages removed", not "builds clean".

## 1. How to build

1. Visual Studio Installer (Visual Studio 2026): workload **Desktop development
   with C++**, plus the individual component **C++ MFC for latest v145 build
   tools (x86 & x64)**. Visual Studio 2026 is internally version 18, so its
   developer shell profiles are labelled "VS 18". To build with Visual Studio
   2022 instead, change `PlatformToolset` in `GrphCalc.vcxproj` back to `v143`.
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
| 14 | (Step 2) `get_fpcw` / `set_fpcw` rewritten from x86 inline assembly (`fstcw` / `fldcw`) to `_controlfp_s`. The save reads the whole control word (mask 0). The restore uses the mask `_MCW_EM \| _MCW_RC \| _MCW_DN`, plus `_MCW_PC` on 32-bit x86 only (`#ifdef _M_IX86`). Return values are checked: if the save fails nothing is restored, and a failed restore is reported with `TRACE` in debug builds | Inline assembly is not available when compiling for x64. The functions save the floating-point settings before the Print dialog and restore them afterwards. On x64 `_controlfp_s` acts on the SSE2 control word (MXCSR) only, and passing the precision or infinity mask invokes the invalid-parameter handler, so those are never passed there [8]. **Behaviour note:** on x64 the exception-mask, rounding and denormal settings are protected; the x87 precision setting is not, because x64 code does not use it. `_controlfp_s` also leaves the denormal-operand exception mask unchanged [8] | `graphview.cpp` |
| 15 | (Step 2) `#undef small` added before `CCalculator::lcm` | The Windows SDK header `rpcndr.h` defines `small` as a macro for `char`, which broke the local variable named `small`. The variable was not renamed (no-renaming rule); the macro is removed for the rest of that one file, which does not use it | `CCalculator.cpp` |

## 3. Project and repository changes

* **New** `GrphCalc.vcxproj` / `.filters`, generated from the file list in
  `GrphCalc.dsp`: 63 sources, shared MFC, multi-byte character set, C++14,
  Debug/Release for x64 and Win32, same nine files excluded from the
  precompiled header as in the original. MBCS is a supported configuration
  again as of Visual Studio 2017 [6].
* Spectre mitigation is **on** in every configuration (`SpectreMitigation` =
  `Spectre`, i.e. `/Qspectre`), by owner decision. This also makes MSBuild
  link the Spectre-mitigated CRT and MFC libraries, so those library
  components must be installed (MSB8040 if not) [7].
* Build isolation (Step 2): `<VcpkgEnabled>false</VcpkgEnabled>` in the Globals
  property group. A user-wide vcpkg integration on the build machine was
  adding its own include folder (outside this repository) to every compile.
  The project uses no vcpkg packages, so the integration is switched off for
  this project only. The vcpkg installation itself was not changed.
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

[7] Microsoft, "/Qspectre," C/C++ compiler options reference. [Online]. Available: https://learn.microsoft.com/en-us/cpp/build/reference/qspectre (accessed Nov. 16, 2026).

[8] Microsoft, "_controlfp_s," C runtime library reference. [Online]. Available: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/controlfp-s (accessed Nov. 16, 2026).

Rows 5, 7, 8, 10 and 11 in section 2 rest on knowledge of current MFC and the
Windows SDK rather than a cited page; the first build will confirm or refute them.

## Step 2 progress log (Debug | x64)

| Date and time | Errors before | Errors after | What was fixed |
|---------------|---------------|--------------|----------------|
| 16 Nov. 2026 14:01 | n/a | 17 | First build that reached the compiler (MSVC 14.52.37025, Spectre libraries). 17 errors, 278 warnings. Two causes: one stray loop variable (`GrphCalcDlg.cpp`), 16 errors from two lines of x86 inline assembly (`graphview.cpp`). |
| 16 Nov. 2026 14:02 | 17 | 16 | `GrphCalcDlg.cpp` line 1092: removed the unused `i = ` from `i = graph_index = GetActiveGraphTabIndex();`. `i` belonged to the earlier `for` loop and its value was never read afterwards, so nothing changes at run time. Same class of fix as section 2 row 3. |
| 16 Nov. 2026 14:02 | 16 | 16 | Stopped for an owner decision: `get_fpcw` / `set_fpcw` in `graphview.cpp` (lines 2586-2599) use x86 inline assembly, which the x64 compiler does not support. |
| 16 Nov. 2026 14:08 | 16 | 8 | `graphview.cpp`: inline assembly replaced with `_controlfp_s` (section 2 row 14). `GrphCalc.vcxproj`: `VcpkgEnabled` = `false` (section 3). All 16 assembly errors gone; 8 new errors appeared in `CCalculator.cpp`, which had not been compiled before because the build stopped earlier. All 8 have one cause (next row). |
| 16 Nov. 2026 14:09 | 8 | 0 | `CCalculator.cpp`: `#undef small` added just before `CCalculator::lcm` (section 2 row 15). **Debug x64 builds and links: 0 errors.** Output: `Legacy Code\Win32\x64\Debug\GrphCalc.exe`. Not yet run. |
| 16 Nov. 2026 14:09 | 0 | 0 | **Release x64 builds and links: 0 errors, 648 warnings** (none fixed, by instruction). By type: C4244 373, LNK4286 128, C4273 92, C4005 46, C4700 4, C4018 3, C4311 1, C4302 1. Output: `Legacy Code\Win32\x64\Release\GrphCalc.exe`. Not yet run. |
