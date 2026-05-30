Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$errors = New-Object System.Collections.Generic.List[string]

$allowedLayers = "app|core|engine|render|editor|export|animation"
$allowedRoles = "entity|state|config|controller|system|service|buffer|shader|validator|fixture|writer|reader|test"
$sourceNamePattern = "^pd_($allowedLayers)_([a-z0-9]+_)*[a-z0-9]+_($allowedRoles)\.(c|h)$"

$forbiddenDirs = @("common", "misc", "helpers", "utils", "manager", "managers")
foreach ($dirName in $forbiddenDirs) {
  $matches = Get-ChildItem -LiteralPath $repoRoot -Directory -Recurse -Force -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -ieq $dirName -and $_.FullName -notmatch "\\\.git\\" }

  foreach ($match in $matches) {
    $errors.Add("Forbidden catch-all directory: $($match.FullName)")
  }
}

$sourceRoots = @("src", "include\prismdraft", "tests")
foreach ($root in $sourceRoots) {
  $fullRoot = Join-Path $repoRoot $root
  if (-not (Test-Path -LiteralPath $fullRoot)) {
    continue
  }

  $files = Get-ChildItem -LiteralPath $fullRoot -File -Recurse |
    Where-Object { $_.Extension -ieq ".c" -or $_.Extension -ieq ".h" }
  foreach ($file in $files) {
    if ($file.Name -notmatch $sourceNamePattern) {
      $errors.Add("Invalid C file name: $($file.FullName). Expected pd_<layer>_<business>_<role>.(c|h).")
    }

    $content = Get-Content -Raw -LiteralPath $file.FullName

    if ($content -match "\b(class|namespace)\s+[A-Za-z_]" -or
        $content -match "\bstd::" -or
        $content -match "\bnew\s+[A-Za-z_]" -or
        $content -match "\bdelete\s+[A-Za-z_]") {
      $errors.Add("Forbidden C++ pattern in C source: $($file.FullName)")
    }

    if ($file.FullName -match "\\src\\core\\" -and
        ($content -match "#\s*include\s*[<""]raylib\.h[>""]" -or
         $content -match "#\s*include\s*[<""]rlgl\.h[>""]" -or
         $content -match "#\s*include\s*[<""]raymath\.h[>""]")) {
      $errors.Add("Core layer must not include Raylib/RLGL/Raymath: $($file.FullName)")
    }
  }
}

if ($errors.Count -gt 0) {
  foreach ($errorItem in $errors) {
    Write-Error $errorItem
  }
  exit 1
}

Write-Host "Architecture check passed."
