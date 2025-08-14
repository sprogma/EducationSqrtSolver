param(
    [string[]]
    $AddCopmilerFlags,
    [string[]]
    $AddLinkerFlags
)

$CopmilerFlags = @("-Wall", "-Wpedantic", "-I.")
$LinkerFlags = @("-Lsqsv", "-lsqsv")

$CopmilerFlags += $AddCopmilerFlags
$LinkerFlags += $AddLinkerFlags

Get-Content -Raw "help.txt" | %{"R""docstring("}{$_}{")docstring"""} | Set-Content ".help.inc"

gcc $CopmilerFlags -c ./main.c -o main.o
gcc $LinkerFlags ./main.o -o "a$($IsWindows ? ".exe" : '')"
