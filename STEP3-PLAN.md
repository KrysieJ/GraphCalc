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

## 3. Licence: one decision needed before code is written

GraphCalc is distributed under the GNU GPL; the file `COPYING` is GPL version 2.
Neither the README nor the source headers say "version 2 only" or "or any later
version".

This matters because many Rust libraries are licensed "MIT OR Apache-2.0". The
FSF considers Apache-2.0 incompatible with GPL version 2 [1], [2]; the Apache
Software Foundation itself acknowledges the FSF has never accepted it as
compatible with GPLv2 [3]. GPL version 3 is compatible with Apache-2.0 [1].

Plan regardless of the answer:

* Licence the port under the GPL, as the original requires for a derivative work.
* Use only dependencies that offer a GPLv2-compatible licence (MIT, BSD, zlib,
  ISC and similar). egui, eframe and egui_plot are "MIT OR Apache-2.0", so the
  MIT option is used [4], [5].
* Enforce this automatically with `cargo-deny`, which fails the build if any
  dependency's licence is outside an allow-list. The same tool checks
  dependencies against the RustSec security-advisory database.

**Owner decision L1:** license the port as "GPL-2.0-or-later" (wider choice of
libraries, if the original terms allow it) or "GPL-2.0-only" (strictest reading).
This is not legal advice; if in doubt, keep GPL-2.0-only and the allow-list above.

## 4. Microsoft Store considerations

* Open-source apps are allowed in the Store when listed by the creator or "a party
  with an appropriate license" [6]. The GPL provides that licence, but the
  listing must include the licence and an offer of source code.
* **Name and branding (owner decision B1):** the original authors (Michael
  Arrison, Brendan Fields) and the graphcalc.com name are not ours. Options:
  ask the original authors for permission to use the name, or publish under a
  new name with "based on GraphCalc" credit.
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

One Cargo workspace in a new top-level folder `rust/` of this repository
(owner decision R1; a separate repository is the alternative).

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
| 0. Foundations | Workspace skeleton, `cargo-deny` licence and advisory checks, golden results captured from the legacy build, `long double` question settled, `CLAUDE.md` updated for Rust | `cargo test` and `cargo deny check` pass on an empty engine; golden files committed |
| 1. Engine | Port `CCalculator` | All engine golden tests pass |
| 2. Scripting | Port `CGCCommand` for the non-graphics commands, with the sandbox | Bundled non-graphics scripts produce golden output |
| 3. App shell | egui window, keypad, output view, menus, options, TOML settings, `.gcl` import, built-in constants and units | Owner smoke test of the calculator |
| 4. 2D graphing | Graph tabs, three graph modes, zoom, all analysis tools, graph script commands | `2dgraph.gcs` behaves as legacy; owner smoke test |
| 5. Help and release | Markdown help, MSIX packaging, Store listing material | Package installs and runs from MSIX on a clean Windows 11 machine |
| Release 2 | 3D graphing (`wgpu`), printing, bitmap and DXF export | |

Work is done by Claude Code in small reviewed batches, as in Step 2, with a
commit after each reviewed batch.

## 10. Open owner decisions

| ID | Question | Recommendation |
|----|----------|----------------|
| L1 | Port licence: GPL-2.0-only or GPL-2.0-or-later? | GPL-2.0-only unless the original terms are confirmed as "or later" |
| B1 | Product name and branding for the Store | Decide before Phase 5; does not block Phases 0 to 4 |
| R1 | Rust code in this repository (`rust/`) or a new repository? | This repository, so legacy and port stay side by side for comparison |
| T1 | Visual style: Windows 11 default dark/light, or a custom theme | Default egui dark and light first; a custom theme can be added in Phase 3 |

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
