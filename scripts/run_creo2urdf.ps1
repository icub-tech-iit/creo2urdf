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


if ([string]::IsNullOrEmpty($env:CREO9_INSTALL_PATH)) {
    Write-Host "CREO9_INSTALL_PATH not set, exiting."
    Exit
}
if ([string]::IsNullOrEmpty($asmPath)) {
    Write-Host "asm path not specified, exiting."
    Exit
}
if ([string]::IsNullOrEmpty($yamlPath)) {
    Write-Host "yaml path not specified, exiting."
    Exit
}
if ([string]::IsNullOrEmpty($csvPath)) {
    Write-Host "csv path not specified, exiting."
    Exit
}
if ([string]::IsNullOrEmpty($outputPath)) {
    Write-Host "output path not specified, exiting."
    Exit
}

$parametricExe = "$env:CREO9_INSTALL_PATH\..\Parametric\bin\parametric.exe"
Start-Process -FilePath $parametricExe -ArgumentList "-g:no_graphics", "-batch_mode", "creo2urdf", "+$asmPath", "+$yamlPath", "+$csvPath", "+$outputPath" -Wait

# Check if model.urdf exists in the output path
$urdfPath = Join-Path -Path $outputPath -ChildPath "model.urdf"
if (Test-Path -Path $urdfPath) {
    Write-Host "Conversion completed successfully. The URDF file is located at: $urdfPath"
}
else {
    Write-Host "Conversion failed. The URDF file was not generated."
}