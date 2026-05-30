Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$cmakePath = & (Join-Path $PSScriptRoot "FindCMake.ps1")
$vsDevCmdPath = & (Join-Path $PSScriptRoot "FindVsDevCmd.ps1")
$buildDir = Join-Path $repoRoot "build/windows-debug"

$command = "`"$vsDevCmdPath`" -arch=x64 && `"$cmakePath`" --build `"$buildDir`" --config Debug --target prismdraft_architecture_check && `"$cmakePath`" --build `"$buildDir`" --config Debug && `"$cmakePath`" --build `"$buildDir`" --config Debug --target test"
cmd /c $command
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}
