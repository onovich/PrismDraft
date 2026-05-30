Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$vsDevCmdPath = & (Join-Path $PSScriptRoot "FindVsDevCmd.ps1")
$buildDir = Join-Path $repoRoot "build/windows-debug"
$exePath = Join-Path $buildDir "prismdraft_phase1_viewport.exe"

& (Join-Path $PSScriptRoot "ConfigureBuild.ps1")
& (Join-Path $PSScriptRoot "Build.ps1")

if (-not (Test-Path -LiteralPath $exePath)) {
  throw "Viewport executable was not found: $exePath"
}

$command = "`"$vsDevCmdPath`" -arch=x64 && cd /d `"$repoRoot`" && `"$exePath`" --interactive"
cmd /c $command
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}
