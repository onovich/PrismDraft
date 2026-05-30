<!-- codex-project-ops-workflow: initialized -->
<!-- initialized-at: 2026-05-30 20:34:40 +08:00 -->

# Codex Ops Workflow

Initialization status: initialized
Project: PrismDraft
Repository root: D:/LabProjects/PrismDraft
Machine config: `.codex\project-ops-workflow.json`
Skill: project-ops-workflow

Treat this document and .codex/project-ops-workflow.json as the source of truth for mechanical project operations.

## Global Wrappers

```
powershell
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\EnvCheck.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\RestoreDeps.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\InstallDeps.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Build.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Test.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Lint.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Format.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Typecheck.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\StructureCheck.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Codegen.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\DocsCheck.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Validate.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\StartDevServer.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\StopDevServer.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Smoke.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Package.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\ReleaseDryRun.cmd
```

## Validate Sequence

lint, typecheck, build, test, structureCheck, docsCheck

## Environment Check

`envCheck` runs:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\FindCMake.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\FindNinja.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\FindVsDevCmd.ps1
```

The finders accept tools on PATH and known Windows Visual Studio tool locations.

## Build

`build` runs:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\ConfigureBuild.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\Build.ps1
```

The build currently configures the minimal C11/CTest scaffold and has no Raylib dependency.

## Test

`test` runs:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\Test.ps1
```

## Structure Check

`structureCheck` runs:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\CheckArchitecture.ps1
```

This guards PrismDraft's required `pd_<layer>_<business>_<role>` source naming, rejects catch-all directories, blocks C++ syntax patterns in C files, and prevents the `core` layer from including Raylib/RLGL/Raymath.

## Dev Server

Start command: ``
Health URL: ``
Ready text: ``
Timeout seconds: 30

## Safety Policy

Do not run destructive clean/reset/deploy commands unless the user explicitly asks.
