# GraphCalc Rebuild

Reviving GraphCalc (GPL-2.0, 1999-2003, Visual C++ 6.0 / MFC) in four steps:

1. Scrub the legacy code so it compiles with a current toolchain. (Done.)
2. Build a working Windows 11 x64 version from the updated code. (Done; dirty-side details in `Legacy Code/`.)
3. Rewrite in Rust and make it publishable in the Microsoft Store. **(Current step; plan in `STEP3-PLAN.md`.)**
4. Design additional features.

## Clean-room rule (read before doing anything)

The Rust engine and script interpreter are a **clean-room rewrite** (owner decision L2), so that new code can carry a new licence instead of GPL-2.0. Every session is one of two kinds, and the kind is fixed for the whole session:

| | Clean-side session | Dirty-side session |
|---|---|---|
| Purpose | Writes Rust code in `rust/` | Writes behaviour specifications in `spec/`, captures golden results, maintains the legacy build |
| May read | `rust/`, `spec/`, `LICENSES/`, this file, `STEP3-PLAN.md` | Everything |
| Must never read | **Anything under `Legacy Code/`**, including its documents, licence file and build logs, and `build.log` at the repository root | (no limit) |
| May write | `rust/` only (plus `LICENSES/`, `REUSE.toml`, `deny.toml` and `.gitignore` during Phase 0) | `spec/` and `Legacy Code/` only; **never `rust/`** |
| Starts in | the `rust\` folder (the repository root only for Phase 0, before `rust\` exists) | the repository root |

- If you are not told which kind you are, you are **clean-side**. Do not open `Legacy Code/` to find out.
- A clean-side session that reads anything under `Legacy Code/`, even by accident or through a search result, must stop, tell the owner, and do no further work in `rust/`. Scope every Grep and Glob to `rust/` or `spec/`; never search the whole repository.
- Clean-side sessions learn behaviour only from `spec/` (including the golden results in `spec/golden/`) and from the owner. If the spec does not cover something, ask the owner; do not guess, and do not reproduce source code remembered from GraphCalc or any other program.
- Dirty-side rules are in `Legacy Code/CLAUDE.md`. Clean-side sessions must not read that file either.

## Licensing and provenance

- `Legacy Code/` is GPL-2.0-only.
- Rust code written on the clean side is **new** and carries `SPDX-License-Identifier: LicenseRef-NewCode-TBD` until the owner chooses the new licence (decision L3).
- Every new file gets an SPDX header (`SPDX-FileCopyrightText` and `SPDX-License-Identifier`), following the REUSE specification. `reuse lint` must pass.
- `rust/PROVENANCE.md` lists every Rust source file with its origin (`new` or `derived`) and the spec sections it implements, plus the table of libraries that replaced legacy components.
- Dependencies must offer a licence that is compatible with GPL-2.0 (for example MIT, BSD, ISC, zlib); `cargo deny check` enforces the allow-list. Never add a dependency that is Apache-2.0-only or GPL-3.0.

## Shell commands (keep permission prompts down)

The owner approves commands by hand, and PowerShell commands containing subexpressions prompt every time, even when an allow rule covers them. So:

- Run one plain command per tool call. Do not chain with `;`, `&&`, `|` or wrap in `$(...)`, `@(...)`, `{...}` script blocks or `ForEach-Object`.
- To inspect logs or source files, use the Read and Grep tools, not `Select-String`, `Get-Content` pipelines or other PowerShell one-liners.
- Use the Edit and Write tools for file changes, never PowerShell or `sed`.

## General rules

- Do not delete files. Do not commit or push unless asked.
- Ask before installing any tool or adding any dependency.
- Default effort is medium. If a problem genuinely needs deeper reasoning, stop and ask the owner to switch to `/effort high` rather than guessing.

## Working with the owner

- The owner is a network engineer, not a programmer. Explain what a change does and why in plain terms; do not expect them to read code.
- Security and accuracy matter more than speed. Say when something is unverified.
- Cite sources for claims about toolchains, APIs or platform behaviour.
- Written documents use IEEE (2006) reference formatting.
