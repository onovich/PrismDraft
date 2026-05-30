Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$command = Get-Command cmake -ErrorAction SilentlyContinue
if ($null -ne $command) {
  Write-Output $command.Source
  exit 0
}

$candidatePaths = @(
  "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\CMake\bin\cmake.exe"
)

foreach ($candidatePath in $candidatePaths) {
  if (Test-Path -LiteralPath $candidatePath) {
    Write-Output $candidatePath
    exit 0
  }
}

Write-Error "CMake was not found on PATH or in known Windows install locations."
exit 1
