param(
    [string[]]
    $AddCopmilerFlags,
    [string[]]
    $AddLinkerFlags
)

pushd $PSScriptRoot

$CopmilerFlags = @("-Wall", "-Wpedantic")
$LinkerFlags = @("-lm")

if (!$IsWindows)
{
    $CompilerFlags += "-fpic"
}

$CopmilerFlags += $AddCopmilerFlags
$LinkerFlags += $AddLinkerFlags

gcc $CopmilerFlags -c ./main.c -o main.o
gcc $LinkerFlags -shared ./main.o -o "$($IsWindows ? '' : "lib")sqsv$($IsWindows ? ".dll" : ".so")"

popd
