$currentpath = $PSScriptRoot
$projectrootpath = (Get-Item $currentpath).parent.parent.FullName

Write-Host $projectrootpath

# Clear project
$cmakefilespath = $projectrootpath + "/temp/cmakefiles"
if (Test-Path -Path $cmakefilespath)
{
    Remove-Item -Recurse -Force -Path $cmakefilespath
} 
else 
{
    $null = New-Item -ItemType Directory -Path $cmakefilespath
}

# Call cmake
(Set-Location $projectrootpath)
(cmake -S . -B "$cmakefilespath" -G "Visual Studio 17 2022" -A x64)
(cmake --open "$cmakefilespath")

Set-Location $currentpath