<#
.SYNOPSIS
    编译项目：先合并源代码为 build/main.cpp，再用 g++ 编译为可执行文件。

.DESCRIPTION
    默认先调用 merge.ps1 生成 build/main.cpp，然后将其编译为 build/<OutName>
    （Windows 下为 build/<OutName>.exe）。

.PARAMETER OutName
    可执行文件名（不含扩展名），默认 main。

.PARAMETER Compiler
    编译器，默认 g++。

.PARAMETER Flags
    编译参数，默认 "-std=c++20 -O2 -Wall -Wextra"。

.PARAMETER NoMerge
    跳过合并步骤，直接编译已存在的 build/main.cpp。
#>
[CmdletBinding()]
param(
    [string]$OutName = 'main',
    [string]$Compiler = 'g++',
    [string]$Flags = '-std=c++20 -O2 -Wall -Wextra',
    [switch]$NoMerge
)

$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
$buildDir = Join-Path $root 'build'
$merged = Join-Path $buildDir 'main.cpp'

if (-not $NoMerge) {
    & (Join-Path $root 'merge.ps1')
}

if (-not (Test-Path -LiteralPath $merged -PathType Leaf)) {
    throw "build: 找不到 $merged，请先运行 merge.ps1"
}

if (-not (Test-Path -LiteralPath $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
}

$onWindows = [System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT
$exeName = if ($onWindows) { $OutName + '.exe' } else { $OutName }
$exePath = Join-Path $buildDir $exeName

$flagList = @($Flags -split '\s+' | Where-Object { $_ -ne '' })
$cmdArgs = @($flagList) + @($merged, '-o', $exePath)

Write-Host ('[build] {0} {1}' -f $Compiler, ($cmdArgs -join ' '))
& $Compiler @cmdArgs
if ($LASTEXITCODE -ne 0) {
    throw "build: 编译失败 (exit code $LASTEXITCODE)"
}

Write-Host ('[build] OK -> {0}' -f $exePath)
