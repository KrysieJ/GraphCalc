# GraphCalc Step 3: Rust port plan

Date: 16 Nov. 2026
Status: **approved by the owner on 16 Nov. 2026, except open decisions L3 and B2
(section 10).** Phase 0a (clean foundations) finished on 17 Nov. 2026; no calculator logic has been written yet.
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
| L2 | **Clean-room rewrite** of the engine and the script interpreter, so the new code can carry the new licence (section 3a) | 16 Nov. 2026 |
| L1a | The C++ patches made in Steps 1 and 2 are modifications of GPL code, not a rewrite; they stay GPL-2.0-only with the rest of `Legacy Code/` | 16 Nov. 2026 |
| L1b | Because the Rust app contains no GPL-2.0 code (L2), **Apache-2.0 dependencies are allowed**. egui's windowing and drawing layers (`winit`, `glutin`, `unicode-general-category` and others) are Apache-2.0-only, and every mainstream Rust GUI uses `winit`. GPL, LGPL and AGPL dependencies are not allowed. The fonts bundled with egui (OFL-1.1, Ubuntu-font-1.0) are allowed for that one crate. No file in `rust/` may be `derived` | 16 Nov. 2026 |

## 2. What has to be ported

Measured from the 165 legacy source files that are built (about 47,600 lines):

| Area | Lines | Treatment | Release |
|------|-------|-----------|---------|
| Calculator engine: expression parser, 64 operators, number bases, formatting, equation solver, min/max, units, constants | 4,400 | Clean-room rewrite from a behaviour spec (L2); this is where correctness matters most | 1 |
| Scripting: about 176 `.gcs` commands, `If`/`ElseIf`/`While`, prompts | 2,000 | Clean-room rewrite from a behaviour spec (L2); sandbox (section 6) | 1 |
| 2D graphing: Euclidean, polar, parametric; zoom, trace, area, tangent, distance, intercepts, intersections, min/max, table, N-slider | 17,700 | Rewrite on `egui_plot`, reusing the engine | 1 |
| Main window, keypad, menus, output view, options, settings I/O | 10,900 | Rewrite in egui | 1 |
| Old third-party MFC widgets (colour picker, hyperlink, data tips, flat edits, sounds) | 4,700 | Drop; egui provides equivalents | 1 |
| 3D graphing (OpenGL) | 6,100 | Rewrite on `wgpu` | 2 |
| Printing, bitmap and DXF export | 2,000 | Rewrite | 2 |

About 70 % of the legacy code is user interface. All of it, engine included, is
rewritten rather than translated (L2). The engine and the script interpreter are
the parts that must behave exactly as before.

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
* Libraries under Apache-2.0-only or GPL-3.0-only cannot go into the same
  program as GPL-2.0 code. (This is why the Rust app must stay free of derived
  code: it uses Apache-2.0-only libraries under decision L1b.) The FSF considers Apache-2.0 incompatible with GPL version 2 [1], [2],
  and the Apache Software Foundation acknowledges this [3]. GPL-2.0-only and
  GPL-3.0 cannot be combined in one binary [13].
* The only ways to ship the whole app under the new licence are: rewrite every
  GPL-derived part without using the original code (chosen: section 3a), or get
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
| `derived` | Rust code translated from or based on legacy code. **Not permitted in `rust/`** (decision L1b) | `GPL-2.0-only` |
| `new` | Written from scratch, no legacy code used | `LicenseRef-NewCode-TBD` (later the chosen licence) |

* The same file has a **library replacement table**: which legacy component
  (MFC dialogs, OpenGL, HTML Help, the colour picker, the INI profile API and so
  on) was replaced by which Rust library, with that library's licence.

**Dependencies (decision L1b).** Allowed: MIT, Apache-2.0, BSD-2/3-Clause, ISC,
Zlib, Unicode-3.0 and CC0-1.0, plus OFL-1.1 and Ubuntu-font-1.0 for egui's
bundled fonts only. Not allowed: any GPL, LGPL or AGPL licence. egui, eframe and
egui_plot are "MIT OR Apache-2.0", and the MIT option is used [4], [5]; several
of their own dependencies are Apache-2.0-only, which is acceptable only because
the app contains no GPL-2.0 code. `rust/PROVENANCE.md` lists them. Apache-2.0
and OFL-1.1 require their licence texts to ship with the app (Phase 5). `cargo-deny` fails the build if any
dependency's licence is outside the allow-list, and also checks dependencies
against the RustSec security-advisory database.

## 3a. Clean-room procedure (decision L2)

Facts and ideas are not protected by copyright; the way they are written in code is.
A clean-room rewrite keeps the two apart: people (here, sessions) who have read the
original write down **what** it does; different people who have never seen it write
**how** from that description alone. Not legal advice.

| Role | Who | Reads | Writes |
|------|-----|-------|--------|
| Dirty side | A Claude Code session started at the repository root, and the reviewing chat | Legacy code, the running Step 2 program | `spec/` (behaviour only, rules in `Legacy Code/CLAUDE.md`), `spec/golden/` |
| Review gate | The owner, helped by the reviewing chat | Each spec chunk | Marks it `Reviewed: yes` only if it contains no code, comments, internal names or code structure |
| Clean side | A separate Claude Code session started in `rust\` | `spec/`, `rust/`, root `CLAUDE.md`, this plan | `rust/` only |

Controls:

* Every Claude Code session starts with no memory of other sessions, so the
  separation is between sessions. A session's side is fixed for its whole life
  (root `CLAUDE.md`).
* Clean-side sessions get a permission **deny rule** for `Legacy Code/` in
  `rust/.claude/settings.json`. Phase 0a tests that the rule actually refuses a
  read; the instruction in `CLAUDE.md` is the backup, not the only control.
* Golden results are behaviour (input and displayed output), so the clean side
  may use them as tests.
* Before release, a dirty-side session compares the Rust code against the legacy
  code for copied structure or text and reports anything suspicious.
* Clean-side sessions never run a full-repository `reuse lint` or repository-wide
  `git` listings, because these read or print `Legacy Code/` paths; they check
  their own files with `reuse lint-file`. The reviewing chat runs the full lint.
* A clean-side session that sees anything from `Legacy Code/` stops, reports it
  and is retired. Incidents are logged below.

**Access block test (Phase 0a, 17 Nov. 2026).** From a session started in
`rust\`, the deny rules refused Read, Grep and Glob of `..\Legacy Code`, a
PowerShell `Get-Content` of a file in it, and a Read of the root `build.log`; a
control Read of this plan was allowed. Rules that worked use absolute paths
(`Read(//**/Legacy Code/**)` and the full `//c/...` form); relative `../` forms
blocked nothing. Remaining gaps: the PowerShell and Bash rules match only
commands containing the text `Legacy Code` (a wildcard such as `..\Legacy*`
would not match); bypass-permissions mode is untested and must not be used on
the clean side; and the block does nothing for a session started anywhere other
than `rust\`. The instruction in `CLAUDE.md` covers these gaps.

**Clean-room incident log.**

| Date | Session | What was seen | Outcome |
|------|---------|---------------|---------|
| 17 Nov. 2026 | Phase 0a clean-side session (started at the repository root) | Two build-output folder names, `Win32/Debug/` and `Win32/x64/`, printed by a `git` listing of ignored files while diagnosing `reuse lint`; no file contents | Standard Visual Studio output-folder names, no protected expression; the Phase 0a work (scaffolding only, no logic) stands. Session retired; Phase 1 starts in a new session from `rust\`. The session's saved lint report, which lists `Legacy Code/` paths, is to be deleted by the owner |

**Known limitation.** GraphCalc's source has been public since the early 2000s,
so the AI model behind both sides may have seen it during training. Two AI
sessions are weaker evidence of independent creation than two separate human
teams. The controls above (behaviour-only spec, access block, similarity check
before release) reduce that risk but do not remove it.

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
| Opening web links could fall back to building and running a command line | Open links only through the standard Windows "open URL" call; no command-line building |
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

Versions at Phase 0a (17 Nov. 2026): `eframe` 0.37.0 and `egui_plot` 0.38.0
(the 0.34 pair originally planned did not match each other), `serde` 1.0.229,
`toml` 1.2.1. `Cargo.lock` is the record.

**Numeric precision.** The legacy engine uses `long double`. In Microsoft's
compiler `long double` uses the same IEEE-754 binary64 representation as
`double`, with the same range and precision [17], and the C runtime's `long double`
functions are identical to their `double` counterparts [18] (confirmed in Phase
0a), so Rust's `f64` should reproduce the
legacy results bit for bit. Phase 0 tests confirm this before the engine port starts.

## 8. Proving the port is correct

The legacy program already contains test material:

* `scripts/Test/engine.gcs` (242 lines), `solver.gcs`, `view.gcs`,
  `2dgraph.gcs`, `3dgraph.gcs` and the other bundled scripts.

Phase 0 runs these through the **Step 2 build** and saves the output as "golden"
results. Every Rust engine and script change is then tested against them: same
input, same displayed answer. New cases are added for edge cases found during the
port (for example `ln(e)` and the min/max search with an upper bound of 0).

## 9. Phases

| Phase | Goal | Done when |
|-------|------|-----------|
| 0a. Clean foundations (clean side) | Workspace skeleton, `cargo-deny` licence and advisory checks, REUSE layout (`LICENSES/`, SPDX headers, `rust/PROVENANCE.md`), `long double` question settled, clean-room access block tested | `cargo test`, `cargo deny check` and `reuse lint` pass on an empty engine; a clean-side session is refused when it tries to read `Legacy Code/` |
| 0b. Specification and golden results (dirty side) | Behaviour spec of the engine and the script language in `spec/`; golden results from the Step 2 build in `spec/golden/`; owner review of each spec chunk | Engine spec reviewed and marked `Reviewed: yes`; golden files committed |
| 1. Engine | Clean-room rewrite of the calculator engine from `spec/` | All engine golden tests pass |
| 2. Scripting | Clean-room rewrite of the script interpreter for the non-graphics commands, with the sandbox | Bundled non-graphics scripts produce golden output |
| 3. App shell | egui window, keypad, output view, menus, options, TOML settings, `.gcl` import, built-in constants and units | Owner smoke test of the calculator |
| 4. 2D graphing | Graph tabs, three graph modes, zoom, all analysis tools, graph script commands | `2dgraph.gcs` behaves as legacy; owner smoke test |
| 5. Help and release | Markdown help, MSIX packaging, Store listing material | Package installs and runs from MSIX on a clean Windows 11 machine |
| Release 2 | 3D graphing (`wgpu`), printing, bitmap and DXF export | |

Work is done by Claude Code in small reviewed batches, as in Step 2, with a
commit after each reviewed batch.

## 10. Open owner decisions

| ID | Question | Notes |
|----|----------|-------|
| L3 | The new-code licence | Owner will decide later. Because the app contains Apache-2.0-only libraries (L1b), it has to be compatible with Apache-2.0: for example MIT, Apache-2.0, MPL-2.0 or GPL-3.0. GPL-2.0-only is ruled out |
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

[10] M. Välimäki, "GNU General Public License and the distribution of derivative works," *Journal of Information, Law and Technology*, 2005(1). [Online]. Available: https://warwick.ac.uk/fac/soc/law/elj/jilt/2005_1/valimaki/ (accessed Nov. 16, 2026). Quotes GPL version 2, section 0.

[11] "GPL and virality," Julia discourse. [Online]. Available: https://discourse.julialang.org/t/gpl-and-virality/7715 (accessed Nov. 16, 2026).

[12] FOSSA, "Is MIT compatible with GPL 2.0?" [Online]. Available: https://fossa.com/resources/devops-tools/license-compatibility-checker/mit-vs-gpl-2-0/ (accessed Nov. 16, 2026).

[13] OSPO Alliance, "What is the difference between the GPL and MIT licences?" [Online]. Available: https://ospo-alliance.org/faq/licences/gpl-vs-mit/ (accessed Nov. 16, 2026).

[14] Free Software Foundation Europe, "Tutorial: How to become REUSE-compliant." [Online]. Available: https://reuse.software/tutorial/ (accessed Nov. 16, 2026).

[15] Free Software Foundation Europe, "reuse-lint(1)." [Online]. Available: https://man.archlinux.org/man/reuse-lint.1.en.txt (accessed Nov. 16, 2026).

[16] Free Software Foundation Europe, "reuse lint," reuse documentation. [Online]. Available: https://reuse.readthedocs.io/en/stable/man/reuse-lint.html (accessed Nov. 16, 2026).

[17] Microsoft, "Conversions from floating-point types," Microsoft Learn. [Online]. Available: https://learn.microsoft.com/cpp/c-language/conversions-from-floating-point-types?view=msvc-170 (accessed Nov. 16, 2026).

[18] Microsoft, "Math and floating-point support," Microsoft Learn. [Online]. Available: https://learn.microsoft.com/cpp/c-runtime-library/floating-point-support?view=msvc-170 (accessed Nov. 16, 2026).
