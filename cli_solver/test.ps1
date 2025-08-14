param(
    $Appliction = "./a.exe"
)

pushd $PSScriptRoot

$test = 0

function Equal
{
    param(
        [double[]]$a,
        [double[]]$b
    )
    if ($a.Length -ne $b.Length)
    {
        Write-Host "Read $($a.Length) roots ($a), but waited $($b.Length) roots. ($b)" -Foreground red
        return $false
    }
    $a = $a | sort
    $b = $b | sort
    for ($i = 0; $i -lt $b.Length; ++$i)
    {
        if ([Math]::abs([double]$a[$i] - [double]$b[$i]) -gt 0.00001)
        {
            Write-Host "At root $($i + 1) found $($a[$i]) [right is $($b[$i])]" -Foreground red
            return $false
        }
    }
    $true
}

function assert
{
    param(
        [scriptblock]$Scriptblock
    )
    
    $script:test++
    if ((& $Scriptblock 2>$null) -eq $false)
    {
        Write-Host "Error. Test N.$test is failed : $Scriptblock" -Foreground red
    }
    else
    {
        Write-Host "Test N.$test passed." -Foreground green
    }
}

assert {Equal (& $Appliction -c 1 3 2) -1, -0.5}
assert {Equal (& $Appliction -c 3 2 1) @()}
assert {Equal (& $Appliction -c 1 2 3) @()}
assert {Equal (& $Appliction -c 3 1 -2) -1, 1.5}
assert {Equal (& $Appliction -c 3 1 -2 -r) -1, (2/3)}
assert {Equal (& $Appliction -c 3 1 0) -3}
assert {Equal (& $Appliction -c 1 -2 1) 1}
assert {Equal ("1`n0`n1" | & $Appliction) 0}
assert {Equal ("1`n1`n0" | & $Appliction) @()}
assert {Equal ("2`n1`n3`n2" | & $Appliction) -1, -0.5}
assert {(& $Appliction -c 0 0 -r) -eq "inf"}
assert {("0`n0" | & $Appliction) -eq "inf"}

popd
