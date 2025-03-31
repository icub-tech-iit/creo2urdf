<#
.SYNOPSIS
    Converter for Creo assembly to URDF format.
.DESCRIPTION
    This script converts  Creo assembly to URDF format using the creo2urdf plugin.
.PARAMETER asmPath
    The path of the .asm file to be converted.
.PARAMETER yamlPath
    The path of the YAML configuration file will be used.
.PARAMETER csvPath
    The path of the CSV configuration file will be used.
.PARAMETER outputPath
    The path where the final output will be saved.
.EXAMPLE
    .\run_creo2urdf.ps1 -asmPath "C:\path\to\asm_file.asm" -yamlPath "C:\path\to\yaml_file.yaml" -csvPath "C:\path\to\csv_file.csv" -outputPath "C:\output\path"
    This command converts the Creo file to URDF format using the specified paths.
#>

param (
    [string]$asmPath,
    [string]$yamlPath,
    [string]$csvPath,
    [string]$outputPath
)

Write-Host "Using asm path: $asmPath"
Write-Host "Using yaml path: $yamlPath"
Write-Host "Using csv path: $csvPath"
Write-Host "Using output path: $outputPath"


if ([string]::IsNullOrEmpty($env:CREO_INSTALL_PATH)) {
    Write-Host "CREO_INSTALL_PATH not set, exiting."
    Exit 1
}
if ([string]::IsNullOrEmpty($asmPath)) {
    Write-Host "asm path not valid, exiting."
    Exit 1
}
if ([string]::IsNullOrEmpty($yamlPath) -or -not (Test-Path $yamlPath -PathType Leaf)) {
    Write-Host "yaml path not valid, exiting."
    Exit 1
}
if ([string]::IsNullOrEmpty($csvPath)-or -not (Test-Path $csvPath -PathType Leaf)) {
    Write-Host "csv path not valid, exiting."
    Exit 1
}
if ([string]::IsNullOrEmpty($outputPath)) {
    Write-Host "output path not specified, exiting."
    Exit 1
}

$parametricExe = "$env:CREO_INSTALL_PATH\..\Parametric\bin\parametric.exe"
$process = Start-Process -FilePath $parametricExe -ArgumentList "-g:no_graphics", "-batch_mode", "creo2urdf", "+$asmPath", "+$yamlPath", "+$csvPath", "+$outputPath" -PassThru -NoNewWindow

Start-Sleep -Seconds 30.0

$process | kill

# Check if model.urdf exists in the output path
$urdfPath = Join-Path -Path $outputPath -ChildPath "model.urdf"
if (Test-Path -Path $urdfPath) {
    Write-Host "Conversion completed successfully. The URDF file is located at: $urdfPath"
    Exit 0
}
else {
    Write-Host "Conversion failed. The URDF file was not generated."
    Exit 1
}
