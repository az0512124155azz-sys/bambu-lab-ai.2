param([Parameter(Mandatory=$true)][string]$Source)
$ErrorActionPreference = "Stop"

$exe = Get-ChildItem -Path $Source -Filter "BambuStudio.exe" -File -Recurse |
    Select-Object -First 1
if (-not $exe) {
    throw "BambuStudio.exe was not found. Extract the complete Windows artifact first, then run the BAT file from its installer folder."
}

$packageRoot = $exe.Directory.FullName
$installRoot = Join-Path $env:LOCALAPPDATA "LayerMind3D"
New-Item -ItemType Directory -Path $installRoot -Force | Out-Null
Copy-Item -Path (Join-Path $packageRoot "*") -Destination $installRoot -Recurse -Force

$installedExe = Join-Path $installRoot $exe.Name
$desktop = [Environment]::GetFolderPath("Desktop")
$shortcutPath = Join-Path $desktop "LayerMind 3D.lnk"
$shell = New-Object -ComObject WScript.Shell
$shortcut = $shell.CreateShortcut($shortcutPath)
$shortcut.TargetPath = $installedExe
$shortcut.WorkingDirectory = $installRoot
$shortcut.Description = "LayerMind 3D - AI modeling inside Bambu Studio"
$shortcut.IconLocation = "$installedExe,0"
$shortcut.Save()

$historyRoot = Join-Path $env:APPDATA "BambuStudio\layermind"
New-Item -ItemType Directory -Path $historyRoot -Force | Out-Null
Write-Host "Installed to $installRoot" -ForegroundColor Green
Write-Host "Chats stay locally under the Bambu Studio user-data folder." -ForegroundColor Green
