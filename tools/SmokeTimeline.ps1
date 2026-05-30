Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildDir = Join-Path $repoRoot "build/windows-debug"
$exePath = Join-Path $buildDir "prismdraft_timeline_demo.exe"

powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot "ConfigureBuild.ps1")
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot "Build.ps1")
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

Push-Location $repoRoot
try {
  & $exePath
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}
finally {
  Pop-Location
}

Write-Output "Timeline smoke passed: sampled PRS pose was applied to app transform state."
