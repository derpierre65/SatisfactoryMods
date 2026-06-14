param (
    [string]$SingleFolder = ""
)

# Self-elevate to admin if not already
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object Security.Principal.WindowsPrincipal($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    $arguments = @(
        "-NoProfile"
        "-ExecutionPolicy", "Bypass"
        "-File", "`"$PSCommandPath`""
    )
    if ($SingleFolder -ne "") {
        $arguments += @("-SingleFolder", "`"$SingleFolder`"")
    }
    Start-Process -FilePath "powershell.exe" -Verb RunAs -ArgumentList $arguments
    exit
}

$SMLPath = "M:\GameDev\Satisfactory\SatisfactoryModLoader\Mods\GameFeatures"
$ModDir = "M:\GameDev\Satisfactory\SatisfactoryMods"

function New-Sym-Link ($Link) {
    Remove-Item ($SMLPath + "\" + $Link) -ErrorAction SilentlyContinue -Recurse
    New-Item -Path ($SMLPath + "\" + $Link) -ItemType SymbolicLink -Value ($ModDir + "\" + $Link) -Force
}

if ($SingleFolder -ne "") {
    $dir = Get-ChildItem $ModDir | Where-Object {$_.PSISContainer -and $_.Name -eq $SingleFolder}
    if ($dir) {
        Write-Output "Processing folder: $($dir.Name)"
        New-Sym-Link $dir.Name
    }
    else {
        Write-Output "Folder '$SingleFolder' not found."
    }
}
else {
    $selected = Get-ChildItem $ModDir -Directory |
        Where-Object { -not $_.Name.StartsWith(".") -and (Get-ChildItem $_.FullName -Filter *.uplugin -File) } |
        Select-Object Name |
        Out-GridView -Title "Select folders for symlink (Ctrl/Shift = multi)" -PassThru

    if (-not $selected) {
        Write-Output "Nothing selected. Abort."
    }
    else {
        foreach ($d in $selected) {
            Write-Output "Processing folder: $($d.Name)"
            New-Sym-Link $d.Name
        }
    }
}