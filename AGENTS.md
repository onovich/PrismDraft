# AGENTS.md

## Global Codex Notes

### Skill Creation And Installation

When creating or updating local Codex skills, make the skill files compatible with both model-side loading and the Codex UI skill picker.

- Write `SKILL.md` and `agents/openai.yaml` as UTF-8 without BOM. PowerShell `Set-Content -Encoding UTF8` can add a BOM on this Windows setup; prefer `.NET` `UTF8Encoding($false)` or verify bytes after writing.
- A skill may work through `$skill-name` while still failing to appear in the UI `@` picker if metadata files contain a BOM or stale plugin cache is being scanned.
- Compare against known-good skills such as `inscape-dev-host-smoke`: both `SKILL.md` and `agents/openai.yaml` should start directly with `---` or `interface:`, not `EF BB BF`.
- For plugin-packaged skills, bump the plugin version/cachebuster, reinstall with the real Codex CLI from `config.toml` (`CODEX_CLI_PATH`), and remove stale cache versions for that plugin when testing UI discovery.
- After installing or changing skills, restart Codex and test both paths: explicit `$SkillName` loading and UI `@SkillName` autocomplete.
- Prefer memorable no-hyphen display names for skills users should invoke from the UI, for example `InitFlow`, `GitFlow`, and `OpsFlow`, while keeping lower-case folder names valid.

Useful byte check on Windows:

```powershell
$bytes = [System.IO.File]::ReadAllBytes('C:\path\to\SKILL.md')
$bytes[0..2] -join ' '
```

If the first three bytes are `239 187 191`, the file has a UTF-8 BOM and should be rewritten without BOM.

## PrismDraft Architecture Guardrails

Before editing source code, Codex must read and follow [docs/codex-architecture-workflow.md](docs/codex-architecture-workflow.md). This workflow is mandatory for layer boundaries, directory layout, file names, C symbol names, topology mutation comments, and forbidden patterns.

Key project constraints:

- Treat [docs/prismdraft-design.md](docs/prismdraft-design.md) and [docs/project-brief.md](docs/project-brief.md) as the product and architecture source of truth.
- Use [docs/project-todo.md](docs/project-todo.md) as the implementation backlog and keep it updated as small transactions are completed.
- Treat [docs/visual-reference.md](docs/visual-reference.md) and the images under `sample/` as the visual target set for modeling style, viewport rendering, and design explanations.
- Keep the core in pure C11 with indexed, contiguous data structures.
- Use the naming grammar `pd_<layer>_<business>_<role>` for C files and public symbols.
- Preserve one-way dependencies: `app -> editor -> render -> engine -> core`; `export -> core`; `animation -> core`.
- Do not add catch-all modules such as `common`, `misc`, `helpers`, `utils`, or `manager`.
- Run `powershell -NoProfile -ExecutionPolicy Bypass -File tools\CheckArchitecture.ps1` when source files or architecture-sensitive directories are changed.
- After each completed small transaction, use `gitflow` / `project-git-workflow` to commit and push the task-scoped changes before starting the next independent transaction.

<!-- codex-init-flow: initialized -->

## Codex Project Workflow

Initialization status: initialized
Initialized at: 2026-05-30 20:34:39 +08:00
Project root: D:\LabProjects\PrismDraft
Initial git remote: git@github.com:onovich/PrismDraft.git

Use these workflow skills for routine Codex work in this project:

- `init-flow`: initialize or refresh this project document and workflow configuration.
- `project-git-workflow` / `git-flow`: use for git status, validation, commit, push, stash, ignore, and guarded discard operations.
- `project-ops-workflow` / `ops-flow`: use for environment checks, dependencies, build, test, lint, format, typecheck, dev server, smoke, package, and release dry-run operations.
- `prismdraft-smoke`: use for PrismDraft viewport smoke checks; prefer `tools\SmokeViewport.cmd` for automated screenshot smoke and `tools\SmokeViewportInteractive.cmd` when the user wants to inspect a visible window.

Prefer the configured wrappers instead of guessing project commands:

```
powershell
C:\Users\Administrator\.codex\skills\project-git-workflow\scripts\git\Status.cmd
C:\Users\Administrator\.codex\skills\project-git-workflow\scripts\git\CommitAndPush.cmd -Message "commit message" -Paths path\to\file,other\file
C:\Users\Administrator\.codex\skills\project-git-workflow\scripts\git\Stash.cmd -StashMessage "reason"
C:\Users\Administrator\.codex\skills\project-git-workflow\scripts\git\DiscardPaths.cmd -ConfirmDangerous -Paths path\to\file
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Validate.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\StartDevServer.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Smoke.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\StopDevServer.cmd
tools\SmokeViewport.cmd
tools\SmokeViewportInteractive.cmd
```

Project-specific workflow configs live at:

- `.codex/project-git-workflow.json`
- `.codex/project-ops-workflow.json`

Do not silently fall back to generic git/build/test behavior when those configs exist. Update this section and the workflow configs deliberately when project policy changes.

<!-- /codex-init-flow -->

