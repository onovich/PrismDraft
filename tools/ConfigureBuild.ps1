Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$cmakePath = & (Join-Path $PSScriptRoot "FindCMake.ps1")
$ninjaPath = & (Join-Path $PSScriptRoot "FindNinja.ps1")
$vsDevCmdPath = & (Join-Path $PSScriptRoot "FindVsDevCmd.ps1")
$raylibSourceDir = & (Join-Path $PSScriptRoot "FindRaylib.ps1")
$buildDir = Join-Path $repoRoot "build/windows-debug"

$command = "`"$vsDevCmdPath`" -arch=x64 && `"$cmakePath`" -S `"$repoRoot`" -B `"$buildDir`" -G Ninja -DCMAKE_MAKE_PROGRAM=`"$ninjaPath`" -DCMAKE_BUILD_TYPE=Debug -DPRISMDRAFT_RAYLIB_SOURCE_DIR=`"$raylibSourceDir`""
cmd /c $command
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}
