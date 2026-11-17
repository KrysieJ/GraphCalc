# GraphCalc Step 3: Rust port plan

Date: 16 Nov. 2026
Status: **draft for owner approval.** No Rust code has been written yet.
Input: the legacy MFC sources in `Legacy Code/Win32`, which now build and run on
Windows 11 x64 (Step 2, see `Legacy Code/Win32/PORTING.md`).

## 1. Owner decisions so far

| # | Decision | Date |
|---|----------|------|
| D1 | GUI: **egui** (via `eframe`), pure Rust | 16 Nov. 2026 |
| D2 | First Store release is **core first**: calculator, 2D graphing with analysis tools, scripting, settings. 3D graphing and printing/export follow in a later release | 16 Nov. 2026 |
| D3 | Scripts stay in the **`.gcs`** format | 16 Nov. 2026 |
| D4 | `constants.dat` and `units.dat` are **modernised**: the built-in data becomes part of the application; only user-added constants and units live in a separate user file | 16 Nov. 2026 |
| D5 | `.gcl` settings: **modernise if it is better for security or performance, otherwise stay compatible** (assessed in section 5) | 16 Nov. 2026 |
| D6 | The legacy `.chm` help is replaced by a modern help system (recorded in Step 2) | 16 Nov. 2026 |
| L1 | Legacy code is treated as **GPL-2.0-only**. New code gets a separate, more modern licence (to be chosen later). Every file records whether it is original, rewritten from the original, or new, and which libraries replaced legacy components (section 3) | 16 Nov. 2026 |
| B1 | The port gets **a new name and branding** (to be chosen later). "RustCalc" and "RustyCalc" are already in use elsewhere and are ruled out | 16 Nov. 2026 |
| R1 | Rust code lives in a `rust/` folder in **this repository** | 16 Nov. 2026 |
| T1 | Visual style: **egui's standard dark and light themes** for now; a custom theme may come later | 16 Nov. 2026 |

## 2. What has to be ported

Measured from the 165 legacy source files that are built (about 47,600 lines):

| Area | Lines | Treatment | Release |
|------|-------|-----------|---------|
| Calculator engine: expression parser, 64 operators, number bases, formatting, equation solver, min/max, units, constants (`CCalculator`) | 4,400 | Port line-by-line in meaning; this is where correctness matters most | 1 |
| Scripting: about 176 `.gcs` commands, `If`/`ElseIf`/`While`, prompts (`CGCCommand`) | 2,000 | Port; sandbox (section 6) | 1 |
| 2D graphing: Euclidean, polar, parametric; zoom, trace, area, tangent, distance, intercepts, intersections, min/max, table, N-slider | 17,700 | Rewrite on `egui_plot`, reusing the engine | 1 |
| Main window, keypad, menus, output view, options, settings I/O | 10,900 | Rewrite in egui | 1 |
| Old third-party MFC widgets (colour picker, hyperlink, data tips, flat edits, sounds) | 4,700 | Drop; egui provides equivalents | 1 |
| 3D graphing (OpenGL) | 6,100 | Rewrite on `wgpu` | 2 |
| Printing, bitmap and DXF export | 2,000 | Rewrite | 2 |

About 70 % of the legacy code is user interface that gets rewritten rather than
translated. The engine and the script interpreter are the parts that must
behave exactly as before.

## 3. Licensing and provenance (decision L1)

Not legal advice; this section records the owner's decision and the constraints
that follow from it.

**Facts.** GraphCalc is distributed under the GNU GPL; the file `COPYING` is GPL
version 2, and the owner has decided to treat it as GPL-2.0-only. The GPL defines
a "work based on the Program" to include a work containing the Program or a
portion of it "translated into another language" [10]. A Rust translation of the
legacy code is therefore a derivative work and stays under GPL-2.0.

**What a separate licence for new code can and cannot do.**

* New files written from scratch can carry their own licence, and keep it when
  someone takes them out of this project, provided that licence is compatible
  with GPL-2.0 (for example MIT). When MIT code and GPL code are combined, the MIT
  part keeps its MIT licence and the combination is distributed under terms that
  satisfy the GPL [11].
* As long as any GPL-2.0 code is in the program, **the program as a whole is
  distributed under GPL-2.0**. The new-code licence applies to the new files, not
  to the shipped app [12].
* Libraries under Apache-2.0-only or GPL-3.0-only still cannot go into the same
  program. The FSF considers Apache-2.0 incompatible with GPL version 2 [1], [2],
  and the Apache Software Foundation acknowledges this [3]. GPL-2.0-only and
  GPL-3.0 cannot be combined in one binary [13].
* The only ways to ship the whole app under the new licence are: rewrite every
  GPL-derived part without using the original code (decision L2 below), or get
  permission from the original copyright holders (Michael Arrison, Brendan Fields).

**How provenance is recorded.** The repository follows the REUSE specification
(Free Software Foundation Europe):

* Each licence's full text goes in a `LICENSES/` folder, named by its SPDX
  identifier, and each file carries an `SPDX-License-Identifier` and
  `SPDX-FileCopyrightText` header [14].
* The `reuse lint` tool reports any file without copyright and licence
  information, so nothing can slip through unlabelled [15].
* Until the new licence is chosen, new files use a placeholder identifier,
  `LicenseRef-NewCode-TBD`. Licence identifiers starting with `LicenseRef-` are
  accepted by the tool [16]. Until it is replaced, new code is not yet licensed
  to anyone else.
* `rust/PROVENANCE.md` lists every Rust source file with one of three origins, and
  the legacy file and function it came from:

| Origin | Meaning | SPDX licence |
|--------|---------|--------------|
| `original` | Legacy code, unchanged or with port fixes (all of `Legacy Code/`) | `GPL-2.0-only` |
| `derived` | Rust code translated from or based on legacy code | `GPL-2.0-only` |
| `new` | Written from scratch, no legacy code used | `LicenseRef-NewCode-TBD` (later the chosen licence) |

* The same file has a **library replacement table**: which legacy component
  (MFC dialogs, OpenGL, HTML Help, the colour picker, the INI profile API and so
  on) was replaced by which Rust library, with that library's licence.

**Dependencies.** Only libraries that offer a GPL-2.0-compatible licence (MIT,
BSD, zlib, ISC and similar). egui, eframe and egui_plot are "MIT OR Apache-2.0",
so the MIT option is used [4], [5]. `cargo-deny` fails the build if any
dependency's licence is outside the allow-list, and also checks dependencies
against the RustSec security-advisory database.

## 4. Microsoft Store considerations

* Open-source apps are allowed in the Store when listed by the creator or "a party
  with an appropriate license" [6]. The GPL provides that licence, but the
  listing must include the licence and an offer of source code.
* **Name and branding (B1):** the port will have a new name; the listing credits
  "based on GraphCalc by Michael Arrison and Brendan Fields". "RustCalc" and
  "RustyCalc" are already in use. Check a shortlist against the Store, GitHub and
  a trademark search before choosing.
* Packaging: the Store requires MSIX. A Rust program can be packaged with
  Microsoft's `winapp` CLI, which has a Rust guide [7], or directly with the
  Windows SDK's `MakeAppx.exe`. The unsigned MSIX is uploaded and Microsoft signs it [8].
* MSIX installs to a read-only folder [9]. The legacy program writes history,
  settings and a file named `temp` into its own folder, so the port writes all
  user data to the user's AppData folder instead.

## 5. Data and file formats

| File | Legacy | Port | Reason |
|------|--------|------|--------|
| `.gcs` scripts | Text, one command per line | **Unchanged** (D3) | Existing and bundled scripts keep working |
| `constants.dat`, `units.dat` | Text files read at start-up from the program folder | **Built into the program** as structured data; user additions in a separate user file in AppData (D4) | Cannot be tampered with or lost; no start-up parsing failure; works in a read-only MSIX folder |
| `.gcl` settings (7 sections, 253 lines) | Windows INI format via the legacy profile API | **New TOML format; legacy `.gcl` can be imported** (D5) | See below |
| Help | `.chm` | Topics converted to Markdown, built into the program, shown in-app (D6) | Works offline, no external viewer |

**Why `.gcl` should be modernised (D5 assessment).** Performance is not a reason;
the file is tiny. Security is:

1. A `.gcl` file can set `startupscript=1` and `scriptfile=<any path>`. Opening a
   settings file someone sent you therefore arms an arbitrary script to run at
   every start-up, and scripts can write files (section 6).
2. The legacy reader has no validation; unknown or out-of-range values are
   accepted silently.

The port reads settings into a typed structure where every value is checked
against its allowed range, and an imported `.gcl` never turns on a start-up
script without asking. Writing back to `.gcl` is not needed, so it is dropped.

## 6. Security design

| Risk in the legacy design | Port |
|---------------------------|------|
| Scripts can write files anywhere (`SaveAs`, `SaveOutput`, `2DSaveBitmap`, `3DSaveBitmap`) and change which settings file and start-up script are used | File-writing commands ask the user for the location the first time a script uses them, or are limited to a user-chosen scripts folder |
| `RunScript` can call scripts recursively; `While` loops have no limit | Depth limit on `RunScript`; iteration limit on loops, with a "keep going?" prompt |
| `HyperLink` falls back to building a command line from the registry and running it with `WinExec` | Open links only through the standard Windows "open URL" call; no command-line building |
| Unsafe C string handling (26 `strcpy`-style calls) | Not applicable in safe Rust; the engine and script crates use `#![forbid(unsafe_code)]` |
| Dependencies | `cargo-deny` for licences and RustSec advisories; `Cargo.lock` committed; no network access at run time |

## 7. Architecture

One Cargo workspace in a new top-level folder `rust/` of this repository (R1).

| Crate | Contents | Depends on |
|-------|----------|------------|
| `gc-engine` | Number type, tokenizer, parser, evaluator, operators, bases, formatting, solver, min/max, area, derivative, units, constants | nothing outside the standard library where possible |
| `gc-script` | `.gcs` reader and interpreter, sandbox rules | `gc-engine` |
| `gc-settings` | TOML settings, `.gcl` import, AppData paths | `serde`, `toml` |
| `gc-app` | egui window, keypad, output view, menus, 2D graph tabs, help viewer | all of the above, `eframe`, `egui_plot` |

**Numeric precision.** The legacy engine uses `long double`. In Microsoft's
compiler `long double` is the same 64-bit type as `double` (to be confirmed in
Phase 0 against Microsoft's documentation), so Rust's `f64` should reproduce the
legacy results bit for bit. Phase 0 tests confirm this before the engine port starts.

## 8. Proving the port is correct

The legacy program already contains test material:

* `scripts/Test/engine.gcs` (242 lines), `solver.gcs`, `view.gcs`,
  `2dgraph.gcs`, `3dgraph.gcs` and the other bundled scripts.
* A built-in `TestFile` routine that evaluates every line of a text file.

Phase 0 runs these through the **Step 2 build** and saves the output as "golden"
results. Every Rust engine and script change is then tested against them: same
input, same displayed answer. New cases are added for edge cases found during the
port (for example `ln(e)` and the min/max search with an upper bound of 0).

## 9. Phases

| Phase | Goal | Done when |
|-------|------|-----------|
| 0. Foundations | Workspace skeleton, `cargo-deny` licence and advisory checks, REUSE layout (`LICENSES/`, SPDX headers, `rust/PROVENANCE.md`), golden results captured from the legacy build, `long double` question settled, `CLAUDE.md` updated for Rust | `cargo test`, `cargo deny check` and `reuse lint` pass on an empty engine; golden files committed |
| 1. Engine | Port `CCalculator` | All engine golden tests pass |
| 2. Scripting | Port `CGCCommand` for the non-graphics commands, with the sandbox | Bundled non-graphics scripts produce golden output |
| 3. App shell | egui window, keypad, output view, menus, options, TOML settings, `.gcl` import, built-in constants and units | Owner smoke test of the calculator |
| 4. 2D graphing | Graph tabs, three graph modes, zoom, all analysis tools, graph script commands | `2dgraph.gcs` behaves as legacy; owner smoke test |
| 5. Help and release | Markdown help, MSIX packaging, Store listing material | Package installs and runs from MSIX on a clean Windows 11 machine |
| Release 2 | 3D graphing (`wgpu`), printing, bitmap and DXF export | |

Work is done by Claude Code in small reviewed batches, as in Step 2, with a
commit after each reviewed batch.

## 10. Open owner decisions

| ID | Question | Notes |
|----|----------|-------|
| L2 | **Before Phase 1:** how are the engine and the script interpreter written? (a) **Translate** from the legacy C++: faster and lowest risk of behaviour differences, but those parts are `derived` and stay GPL-2.0-only. (b) **Clean-room**: one session writes a behaviour specification from the legacy code and golden results; a separate Claude Code session that never opens `Legacy Code/` implements from the specification and tests only. Slower, and the separation has to be kept strictly, but those parts can be `new` | Only (b) or the original authors' permission lets the whole app move to the new licence later. The UI is rewritten from scratch either way, so it is `new` |
| L3 | The new-code licence | Owner will decide later. It has to be GPL-2.0-compatible (MIT, BSD-2/3-Clause, zlib, ISC) for as long as the app contains GPL-2.0 code |
| B2 | New product name | Before Phase 5; does not block Phases 0 to 4 |

## References

[1] Free Software Foundation, "License compatibility and relicensing." [Online]. Available: https://www.gnu.org/licenses/license-compatibility (accessed Nov. 16, 2026).

[2] FOSSA, "Is Apache 2.0 compatible with GPL 2.0?" [Online]. Available: https://fossa.com/resources/license-compliance-tools/license-compatibility-checker/apache-2-0-vs-gpl-2-0/ (accessed Nov. 16, 2026).

[3] Apache Software Foundation, "Apache License v2.0 and GPL compatibility." [Online]. Available: https://apache.org/licenses/GPL-compatibility.html (accessed Nov. 16, 2026).

[4] egui project, "egui 0.34.2 README," docs.rs. [Online]. Available: https://docs.rs/crate/egui/0.34.2/source/README.md (accessed Nov. 16, 2026).

[5] ecosyste.ms, "egui_plot 0.34.1 package metadata." [Online]. Available: https://packages.ecosyste.ms/api/v1/registries/crates.io/packages/egui_plot/codemeta (accessed Nov. 16, 2026).

[6] Windows Central, "Microsoft removes restrictions on 'generally free' and open-source apps in Microsoft Store." [Online]. Available: https://www.windowscentral.com/microsoft/microsoft-removes-restrictions-on-generally-free-and-open-source-apps-in-microsoft-store (accessed Nov. 16, 2026).

[7] Microsoft, "Use the winapp CLI with a Rust app." [Online]. Available: https://learn.microsoft.com/en-us/windows/apps/dev-tools/winapp-cli/guides/rust (accessed Nov. 16, 2026).

[8] I. Packard, "Building WSL-UI: The Microsoft Store journey," DEV Community. [Online]. Available: https://dev.to/octasoft-ltd/building-wsl-ui-the-microsoft-store-journey-2428 (accessed Nov. 16, 2026).

[9] Advanced Installer, "MSIX introduction." [Online]. Available: https://www.advancedinstaller.com/msix-introduction (accessed Nov. 16, 2026).
