Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$candidatePaths = @(
  "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat",
  "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)

foreach ($candidatePath in $candidatePaths) {
  if (Test-Path -LiteralPath $candidatePath) {
    Write-Output $candidatePath
    exit 0
  }
}

Write-Error "VsDevCmd.bat was not found in known Windows Visual Studio locations."
exit 1
