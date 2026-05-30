Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not [string]::IsNullOrWhiteSpace($env:RAYLIB_SOURCE_DIR)) {
  $envPath = Resolve-Path -LiteralPath $env:RAYLIB_SOURCE_DIR -ErrorAction Stop
  if (Test-Path -LiteralPath (Join-Path $envPath "src\raylib.h")) {
    Write-Output $envPath.Path
    exit 0
  }
}

$candidatePaths = @(
  "D:\CProject\raylib"
)

foreach ($candidatePath in $candidatePaths) {
  if (Test-Path -LiteralPath (Join-Path $candidatePath "src\raylib.h")) {
    Write-Output $candidatePath
    exit 0
  }
}

Write-Error "Raylib source was not found. Set RAYLIB_SOURCE_DIR to a raylib checkout containing src\raylib.h."
exit 1
