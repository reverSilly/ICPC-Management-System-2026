<#
.SYNOPSIS
    编译并运行测试：先调用 build.ps1 编译，再用 data 目录下的 *.in/*.out 逐点评测。

.DESCRIPTION
    对 data 目录下每个 *.in 用例运行编译产物，将标准输出与同名 *.out 比较。
    比较时会忽略行尾空白与末尾空行、统一换行符，以适应常见的 OJ 判题口径。
    失败用例的实际输出会保存到 build/test/<case>.actual.out 以便排查。

.PARAMETER ExeName
    可执行文件名（不含扩展名），默认 main，需与 build.ps1 保持一致。

.PARAMETER DataDir
    测试数据目录，默认 <项目根>/data。

.PARAMETER NoBuild
    跳过编译，直接用已有的可执行文件评测。
#>
[CmdletBinding()]
param(
    [string]$ExeName = 'main',
    [string]$DataDir,
    [switch]$NoBuild
)

$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
if (-not $DataDir) { $DataDir = Join-Path $root 'data' }

if (-not $NoBuild) {
    & (Join-Path $root 'build.ps1') -OutName $ExeName
}

$onWindows = [System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT
$exeName = if ($onWindows) { $ExeName + '.exe' } else { $ExeName }
$exePath = Join-Path (Join-Path $root 'build') $exeName

if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
    throw "test: 找不到可执行文件 $exePath"
}
if (-not (Test-Path -LiteralPath $DataDir -PathType Container)) {
    throw "test: 找不到测试数据目录 $DataDir"
}

# 统一换行符、去除行尾空白与末尾空行
function Normalize-Text([string]$text) {
    if ($null -eq $text) { return '' }
    $text = $text -replace "`r`n", "`n"
    $lines = $text -split "`n"
    for ($i = 0; $i -lt $lines.Count; $i++) { $lines[$i] = $lines[$i].TrimEnd() }
    $end = $lines.Count - 1
    while ($end -ge 0 -and $lines[$end] -eq '') { $end-- }
    if ($end -lt 0) { return '' }
    return ($lines[0..$end] -join "`n")
}

# 返回首个不同行的行号（从 1 开始），完全相同返回 0
function Get-FirstDiffLine([string]$a, [string]$b) {
    $la = $a -split "`n"
    $lb = $b -split "`n"
    $n = [Math]::Max($la.Count, $lb.Count)
    for ($i = 0; $i -lt $n; $i++) {
        $x = if ($i -lt $la.Count) { $la[$i] } else { '<EOF>' }
        $y = if ($i -lt $lb.Count) { $lb[$i] } else { '<EOF>' }
        if ($x -ne $y) { return ($i + 1) }
    }
    return 0
}

$cases = @(Get-ChildItem -LiteralPath $DataDir -File -Filter '*.in' | Sort-Object Name)
if ($cases.Count -eq 0) {
    throw "test: $DataDir 下没有 *.in 测试数据"
}

$failDir = Join-Path (Join-Path $root 'build') 'test'
$passCount = 0
$failCount = 0
$totalWatch = [System.Diagnostics.Stopwatch]::StartNew()

foreach ($case in $cases) {
    $caseName = [System.IO.Path]::GetFileNameWithoutExtension($case.Name)
    $inFile = $case.FullName
    $expectedFile = [System.IO.Path]::ChangeExtension($inFile, '.out')

    if (-not (Test-Path -LiteralPath $expectedFile -PathType Leaf)) {
        Write-Host ('[skip] {0}: 缺少 {1}' -f $caseName, [System.IO.Path]::GetFileName($expectedFile)) -ForegroundColor Yellow
        continue
    }

    $actualFile = Join-Path ([System.IO.Path]::GetTempPath()) ('trae-test-' + [guid]::NewGuid().ToString('N') + '.out')
    $watch = [System.Diagnostics.Stopwatch]::StartNew()
    $proc = Start-Process -FilePath $exePath -RedirectStandardInput $inFile -RedirectStandardOutput $actualFile -NoNewWindow -Wait -PassThru
    $watch.Stop()

    if ($proc.ExitCode -ne 0) {
        $failCount++
        Write-Host ('[FAIL] {0}: 运行时异常退出 (exit code {1}, {2:N0} ms)' -f $caseName, $proc.ExitCode, $watch.Elapsed.TotalMilliseconds) -ForegroundColor Red
    }
    else {
        $expected = Normalize-Text ([System.IO.File]::ReadAllText($expectedFile))
        $actual = Normalize-Text ([System.IO.File]::ReadAllText($actualFile))
        if ($expected -eq $actual) {
            $passCount++
            Write-Host ('[PASS] {0} ({1:N0} ms)' -f $caseName, $watch.Elapsed.TotalMilliseconds) -ForegroundColor Green
        }
        else {
            $failCount++
            $line = Get-FirstDiffLine $expected $actual
            Write-Host ('[FAIL] {0}: 输出不一致，首个不同在第 {1} 行 ({2:N0} ms)' -f $caseName, $line, $watch.Elapsed.TotalMilliseconds) -ForegroundColor Red
            if (-not (Test-Path -LiteralPath $failDir)) {
                New-Item -ItemType Directory -Path $failDir -Force | Out-Null
            }
            Copy-Item -LiteralPath $actualFile -Destination (Join-Path $failDir ($caseName + '.actual.out')) -Force
        }
    }

    Remove-Item -LiteralPath $actualFile -Force -ErrorAction SilentlyContinue
}

$totalWatch.Stop()
$summaryColor = if ($failCount -eq 0) { 'Green' } else { 'Red' }
Write-Host ''
Write-Host ('[test] {0} passed, {1} failed, total {2:N0} ms' -f $passCount, $failCount, $totalWatch.Elapsed.TotalMilliseconds) -ForegroundColor $summaryColor
if ($failCount -gt 0) {
    Write-Host ('[test] 失败用例的实际输出已保存到 {0}' -f $failDir)
    exit 1
}
