Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$cmakePath = & (Join-Path $PSScriptRoot "FindCMake.ps1")
$vsDevCmdPath = & (Join-Path $PSScriptRoot "FindVsDevCmd.ps1")
$buildDir = Join-Path $repoRoot "build/windows-debug"
$testPattern = "pd_core_face_bevel_controller_test|pd_core_face_loop_cut_controller_test|pd_editor_modeling_service_test|pd_editor_visual_state_test"

& (Join-Path $PSScriptRoot "ConfigureBuild.ps1")
& (Join-Path $PSScriptRoot "Build.ps1")

$command = "`"$vsDevCmdPath`" -arch=x64 && `"$cmakePath`" --build `"$buildDir`" --config Debug --target pd_core_face_bevel_controller_test pd_core_face_loop_cut_controller_test pd_editor_modeling_service_test pd_editor_visual_state_test && ctest --test-dir `"$buildDir`" -C Debug -R `"$testPattern`" --output-on-failure"
cmd /c $command
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

& (Join-Path $PSScriptRoot "SmokeViewport.cmd")
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}
