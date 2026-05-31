param(
  [Parameter(ValueFromRemainingArguments = $true)]
  [string[]]$ViewportArgs = @()
)

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

$argumentText = ""
if ($ViewportArgs.Count -gt 0) {
  $quotedArgs = $ViewportArgs | ForEach-Object { "`"$($_ -replace '"', '\"')`"" }
  $argumentText = " " + ($quotedArgs -join " ")
}

$command = "`"$vsDevCmdPath`" -arch=x64 && cd /d `"$repoRoot`" && `"$exePath`"$argumentText"
cmd /c $command
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

$captureName = "phase2_cube"
$caseArgumentIndex = [Array]::IndexOf($ViewportArgs, "--smoke-case")
if ($caseArgumentIndex -ge 0 -and $caseArgumentIndex -lt ($ViewportArgs.Count - 1)) {
  $captureName = $ViewportArgs[$caseArgumentIndex + 1]
}

$capturePath = Join-Path $repoRoot "captures/$captureName.png"
if (-not (Test-Path -LiteralPath $capturePath)) {
  throw "Expected screenshot was not produced: $capturePath"
}

Write-Host "Captured $capturePath"
