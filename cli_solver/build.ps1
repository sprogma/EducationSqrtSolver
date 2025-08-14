param(
    [string[]]
    $AddCompilerFlags,
    [string[]]
    $AddLinkerFlags
)

$CompilerFlags = @("-Wall", "-I.", "-std=gnu11")
$LinkerFlags = @("-Lsqsv", "-lsqsv")

$CompilerFlags += $AddCompilerFlags
$LinkerFlags += $AddLinkerFlags

Get-Content -Raw "help.txt" | %{"R""docstring("}{$_}{")docstring"""} | Set-Content ".help.inc"

gcc $CompilerFlags -c ./main.c -o main.o
gcc $LinkerFlags ./main.o -o "a$($IsWindows ? ".exe" : '')"

if ($IsWindows)
{
    cp ./sqsv/sqsv.dll ./
}
