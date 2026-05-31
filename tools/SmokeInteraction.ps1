Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$cmakePath = & (Join-Path $PSScriptRoot "FindCMake.ps1")
$vsDevCmdPath = & (Join-Path $PSScriptRoot "FindVsDevCmd.ps1")
$buildDir = Join-Path $repoRoot "build/windows-debug"
$exePath = Join-Path $buildDir "prismdraft_phase1_viewport.exe"
$testPattern = "pd_core_face_bevel_controller_test|pd_core_face_loop_cut_controller_test|pd_editor_modeling_service_test|pd_editor_visual_state_test"

function Invoke-ViewportCase {
  param([string]$CaseName)

  $command = "`"$vsDevCmdPath`" -arch=x64 && cd /d `"$repoRoot`" && `"$exePath`" --smoke-case $CaseName"
  cmd /c $command 2>&1 | ForEach-Object { Write-Host $_ }
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }

  $capturePath = Join-Path $repoRoot "captures/$CaseName.png"
  if (-not (Test-Path -LiteralPath $capturePath)) {
    throw "Expected screenshot was not produced: $capturePath"
  }

  Write-Output $capturePath
}

function Get-PngDifferenceStats {
  param(
    [string]$LeftPath,
    [string]$RightPath
  )

  Add-Type -AssemblyName System.Drawing
  $leftBitmap = [System.Drawing.Bitmap]::new($LeftPath)
  $rightBitmap = [System.Drawing.Bitmap]::new($RightPath)
  try {
    $width = [Math]::Min($leftBitmap.Width, $rightBitmap.Width)
    $height = [Math]::Min($leftBitmap.Height, $rightBitmap.Height)
    $totalDifference = 0.0
    $sampleCount = 0
    $changedCount = 0
    $maxDifference = 0.0

    for ($y = 0; $y -lt $height; $y += 12) {
      for ($x = 0; $x -lt $width; $x += 12) {
        $leftPixel = $leftBitmap.GetPixel($x, $y)
        $rightPixel = $rightBitmap.GetPixel($x, $y)
        $pixelDifference = (
          [Math]::Abs($leftPixel.R - $rightPixel.R) +
          [Math]::Abs($leftPixel.G - $rightPixel.G) +
          [Math]::Abs($leftPixel.B - $rightPixel.B)
        ) / 3.0
        $totalDifference += $pixelDifference
        $sampleCount++
        if ($pixelDifference -gt 2.0) {
          $changedCount++
        }
        if ($pixelDifference -gt $maxDifference) {
          $maxDifference = $pixelDifference
        }
      }
    }

    if ($sampleCount -eq 0) {
      throw "No pixels were sampled for comparison."
    }

    return [pscustomobject]@{
      Average = $totalDifference / $sampleCount
      ChangedRatio = $changedCount / $sampleCount
      Max = $maxDifference
    }
  }
  finally {
    $leftBitmap.Dispose()
    $rightBitmap.Dispose()
  }
}

function Assert-VisualDifference {
  param(
    [string]$Label,
    [string]$LeftPath,
    [string]$RightPath,
    [double]$MinimumAverage,
    [double]$MinimumChangedRatio,
    [double]$MinimumMax
  )

  $stats = Get-PngDifferenceStats -LeftPath $LeftPath -RightPath $RightPath
  if ($stats.Average -lt $MinimumAverage -or
      $stats.ChangedRatio -lt $MinimumChangedRatio -or
      $stats.Max -lt $MinimumMax) {
    throw "$Label visual difference was too small: avg=$($stats.Average), ratio=$($stats.ChangedRatio), max=$($stats.Max)"
  }

  Write-Host "$Label visual difference passed: avg=$($stats.Average), ratio=$($stats.ChangedRatio), max=$($stats.Max)"
}

& (Join-Path $PSScriptRoot "ConfigureBuild.ps1")
& (Join-Path $PSScriptRoot "Build.ps1")

$command = "`"$vsDevCmdPath`" -arch=x64 && `"$cmakePath`" --build `"$buildDir`" --config Debug --target pd_core_face_bevel_controller_test pd_core_face_loop_cut_controller_test pd_editor_modeling_service_test pd_editor_visual_state_test && ctest --test-dir `"$buildDir`" -C Debug -R `"$testPattern`" --output-on-failure"
cmd /c $command
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

$defaultPath = Invoke-ViewportCase -CaseName "visual-default"
$edgeLowPath = Invoke-ViewportCase -CaseName "visual-edge-low"
$edgeHighPath = Invoke-ViewportCase -CaseName "visual-edge-high"
$normalLowPath = Invoke-ViewportCase -CaseName "visual-normal-low"
$normalHighPath = Invoke-ViewportCase -CaseName "visual-normal-high"
$darkLowPath = Invoke-ViewportCase -CaseName "visual-dark-low"
$darkHighPath = Invoke-ViewportCase -CaseName "visual-dark-high"
$transformMoveRightPath = Invoke-ViewportCase -CaseName "transform-move-right"
$shadowSinkAboveGroundPath = Invoke-ViewportCase -CaseName "shadow-sink-above-ground"
$shadowSinkBelowGroundPath = Invoke-ViewportCase -CaseName "shadow-sink-below-ground"
$cameraPerspectivePath = Invoke-ViewportCase -CaseName "camera-perspective"
$groundHiddenPath = Invoke-ViewportCase -CaseName "ground-hidden"
$groundColorBluePath = Invoke-ViewportCase -CaseName "ground-color-blue"
$groundMoveUpPath = Invoke-ViewportCase -CaseName "ground-move-up"
$sceneTwoCubesPath = Invoke-ViewportCase -CaseName "scene-two-cubes"
$sceneTwoCubesMoveSecondPath = Invoke-ViewportCase -CaseName "scene-two-cubes-move-second"
$bevelPath = Invoke-ViewportCase -CaseName "modeling-bevel"
$loopCutPath = Invoke-ViewportCase -CaseName "modeling-loop-cut"

Assert-VisualDifference -Label "edge threshold" -LeftPath $edgeLowPath -RightPath $edgeHighPath -MinimumAverage 0.15 -MinimumChangedRatio 0.001 -MinimumMax 30.0
Assert-VisualDifference -Label "normal threshold" -LeftPath $normalLowPath -RightPath $normalHighPath -MinimumAverage 0.15 -MinimumChangedRatio 0.001 -MinimumMax 30.0
Assert-VisualDifference -Label "dark intensity" -LeftPath $darkLowPath -RightPath $darkHighPath -MinimumAverage 2.0 -MinimumChangedRatio 0.05 -MinimumMax 10.0
Assert-VisualDifference -Label "transform move" -LeftPath $defaultPath -RightPath $transformMoveRightPath -MinimumAverage 2.0 -MinimumChangedRatio 0.03 -MinimumMax 20.0
Assert-VisualDifference -Label "shadow sink through ground" -LeftPath $shadowSinkAboveGroundPath -RightPath $shadowSinkBelowGroundPath -MinimumAverage 0.2 -MinimumChangedRatio 0.005 -MinimumMax 10.0
Assert-VisualDifference -Label "camera perspective" -LeftPath $defaultPath -RightPath $cameraPerspectivePath -MinimumAverage 1.0 -MinimumChangedRatio 0.02 -MinimumMax 20.0
Assert-VisualDifference -Label "ground hidden" -LeftPath $defaultPath -RightPath $groundHiddenPath -MinimumAverage 1.0 -MinimumChangedRatio 0.02 -MinimumMax 20.0
Assert-VisualDifference -Label "ground color" -LeftPath $defaultPath -RightPath $groundColorBluePath -MinimumAverage 1.0 -MinimumChangedRatio 0.02 -MinimumMax 20.0
Assert-VisualDifference -Label "ground move" -LeftPath $defaultPath -RightPath $groundMoveUpPath -MinimumAverage 0.5 -MinimumChangedRatio 0.01 -MinimumMax 10.0
Assert-VisualDifference -Label "scene create cube" -LeftPath $defaultPath -RightPath $sceneTwoCubesPath -MinimumAverage 1.0 -MinimumChangedRatio 0.02 -MinimumMax 20.0
Assert-VisualDifference -Label "scene move second cube" -LeftPath $sceneTwoCubesPath -RightPath $sceneTwoCubesMoveSecondPath -MinimumAverage 1.0 -MinimumChangedRatio 0.02 -MinimumMax 20.0
Assert-VisualDifference -Label "bevel modeling" -LeftPath $defaultPath -RightPath $bevelPath -MinimumAverage 0.15 -MinimumChangedRatio 0.001 -MinimumMax 20.0
Assert-VisualDifference -Label "loop cut modeling" -LeftPath $defaultPath -RightPath $loopCutPath -MinimumAverage 0.15 -MinimumChangedRatio 0.001 -MinimumMax 20.0
