param(
    [string]$UnrealRoot = $env:UE_ROOT,
    [string]$Configuration = "Development"
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
$project = Join-Path $repoRoot "BDFR_DriveCore.uproject"

if (-not (Test-Path $project)) { throw "Project not found: $project" }
if (-not $UnrealRoot) {
    $UnrealRoot = @(
        "C:\Program Files\Epic Games\UE_5.8",
        "D:\Epic Games\UE_5.8",
        "D:\Program Files\Epic Games\UE_5.8"
    ) | Where-Object { Test-Path $_ } | Select-Object -First 1
}
if (-not $UnrealRoot) { throw "Unreal Engine 5.8 not found. Pass -UnrealRoot or set UE_ROOT." }

$build = Join-Path $UnrealRoot "Engine\Build\BatchFiles\Build.bat"
if (-not (Test-Path $build)) { throw "Build.bat not found under $UnrealRoot" }

& $build "BDFR_DriveCoreEditor" Win64 $Configuration $project -WaitMutex -NoHotReloadFromIDE
if ($LASTEXITCODE -ne 0) { throw "BDFR_DriveCoreEditor build failed with exit code $LASTEXITCODE." }
Write-Host "BDFR_DriveCoreEditor UE 5.8 build succeeded."
