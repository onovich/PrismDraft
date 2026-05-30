Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildDir = Join-Path $repoRoot "build/windows-debug"
$exePath = Join-Path $buildDir "prismdraft_export_smoke.exe"
$gltfPath = Join-Path $repoRoot "captures/phase6_export_smoke.gltf"
$glbPath = Join-Path $repoRoot "captures/phase6_export_smoke.glb"

powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot "ConfigureBuild.ps1")
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot "Build.ps1")
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

New-Item -ItemType Directory -Force -Path (Join-Path $repoRoot "captures") | Out-Null

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

$gltfItem = Get-Item -LiteralPath $gltfPath
$glbItem = Get-Item -LiteralPath $glbPath

if ($gltfItem.Length -le 0 -or $glbItem.Length -le 0) {
  Write-Error "Export smoke failed: output files are empty."
  exit 1
}

Write-Output "Export smoke passed: $($gltfItem.FullName) ($($gltfItem.Length) bytes), $($glbItem.FullName) ($($glbItem.Length) bytes)"
