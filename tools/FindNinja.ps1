Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$command = Get-Command ninja -ErrorAction SilentlyContinue
if ($null -ne $command) {
  Write-Output $command.Source
  exit 0
}

$candidatePaths = @(
  "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
)

foreach ($candidatePath in $candidatePaths) {
  if (Test-Path -LiteralPath $candidatePath) {
    Write-Output $candidatePath
    exit 0
  }
}

Write-Error "Ninja was not found on PATH or in known Windows Visual Studio locations."
exit 1
