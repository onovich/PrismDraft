Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildDir = Join-Path $repoRoot "build/windows-debug"
$exePath = Join-Path $buildDir "prismdraft_phase1_viewport.exe"

& (Join-Path $PSScriptRoot "ConfigureBuild.ps1")
& (Join-Path $PSScriptRoot "Build.ps1")

if (-not (Test-Path -LiteralPath $exePath)) {
  throw "Viewport executable was not found: $exePath"
}

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public static class PdViewportLifecycleSmokeNative
{
    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr SendMessage(IntPtr hWnd, UInt32 msg, IntPtr wParam, IntPtr lParam);
}
"@

$process = Start-Process -FilePath $exePath -ArgumentList "--interactive" -WorkingDirectory $repoRoot -PassThru
$windowHandle = [IntPtr]::Zero
$deadline = (Get-Date).AddSeconds(15)

try {
  while ((Get-Date) -lt $deadline) {
    if ($process.HasExited) {
      throw "Viewport process exited before a window handle was available. Exit code: $($process.ExitCode)"
    }

    $process.Refresh()
    if ($process.MainWindowHandle -ne [IntPtr]::Zero) {
      $windowHandle = $process.MainWindowHandle
      break
    }

    Start-Sleep -Milliseconds 100
  }

  if ($windowHandle -eq [IntPtr]::Zero) {
    throw "Viewport window handle was not available before timeout."
  }

  $wmClose = 0x0010
  [void][PdViewportLifecycleSmokeNative]::SendMessage($windowHandle, $wmClose, [IntPtr]::Zero, [IntPtr]::Zero)

  if (-not $process.WaitForExit(10000)) {
    throw "Viewport process did not exit after WM_CLOSE."
  }

  Write-Host "Viewport lifecycle smoke passed. PID $($process.Id) exited after WM_CLOSE with code $($process.ExitCode)."
}
finally {
  if (-not $process.HasExited) {
    Stop-Process -Id $process.Id -Force
  }
}
