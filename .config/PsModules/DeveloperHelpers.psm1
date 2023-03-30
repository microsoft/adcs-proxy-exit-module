<#
.SYNOPSIS
Add PowerShell functions here which help the local developer experience
- Developer can add these commands to their PS environment by running AddModules.ps1 at the root
- These functions are not available on remote builds
#>

<#
.SYNOPSIS
Refresh credentials to the NuGet feeds
This allows the credential provider to use an interactive prompt which msbuild /t:restore doesn't do
#>
function Update-NuGetCredentials()
{
    $nugetConfig = "$PSScriptRoot\..\..\nuget.config"
    $credentialManager = "$PSScriptRoot\..\..\build\local\nuget\CredentialProvider.VSS.exe"
    [xml] $xml = Get-Content $nugetConfig
    $sources = $xml.Configuration.PackageSources.add.value
    
    foreach ($source in $sources)
    {
        Write-Host "Refreshing credentials for $source"
        $output = Invoke-Expression "$credentialManager -u $source 2>&1"
        
        if ($lastExitCode -eq 0)
        {
            Write-Host "Success"
        }
        else
        {
            Write-Error "Credential Provider failed"
            Write-Host $output
        }
    }
}

<#
.SYNOPSIS
Invokes MSBuild with some default arguments:
1. N-1 degrees of parallelism. Speeds up the build but doesn't kill your computer
2. Log to a file (msbuild.log)
#>
function Invoke-MSBuild([switch]$Release, [switch]$SkipProjectReferences, [switch]$Investigate) {
    $parallel = $env:NUMBER_OF_PROCESSORS - 1
    # explicitly find a *.*proj to avoid invoking it on an sln
    $proj = (Get-ChildItem *.*proj)[0]
    $configuration = "Debug"
    if ($release) {
        $configuration = "Release"
    }
    $skipReferences = "true"
    if ($SkipProjectReferences) {
        $skipReferences = "false"
    }

    if ($Investigate) {
        msbuild /maxcpucount:$parallel /v:diag /bl:log.binlog /filelogger /property:BuildProjectReferences=$skipReferences /property:Configuration=$configuration $proj /consoleloggerparameters:Verbosity=normal
    } else {
        msbuild /maxcpucount:$parallel /v:normal /filelogger /property:BuildProjectReferences=$skipReferences /property:Configuration=$configuration $proj
    }
}

<#
.SYNOPSIS
Forces a NuGet restore by cleaning the CBT 'build' folder and then calling msbuild /t:restore

MSBuild doesn't eagerly restore packages, so if you do a git pull and someone updated a package,
MSBuild will not download the package unless you do something like clean the 'build' folder.
#>
function Restore-Packages()
{
    $buildFolder = Join-Path $PSScriptRoot "..\..\build"
    pushd $buildFolder
    Write-Host "Cleaning 'build' folder"
    git clean -xfd
    popd
    pushd (Resolve-Path (Join-Path $PSScriptRoot "..\..\"))
    Write-Host "Begin restore"
    msbuild /t:Restore
    popd
}

<#
.SYNOPSIS
Cleans the repo without wiping out NuGet packages.

Will check for uncommited files to prevent losing work.
#>
function Reset-Repo()
{
    pushd (Resolve-Path (Join-Path $PSScriptRoot "..\..\"))
    $changes = git status -s
    if ($null -ne $changes) {
        Write-Host "!!!WARNING: You have uncommited files" -ForegroundColor 'Yellow'
        git status
        $response = Read-Host "Proceed? (y/n)"
        if ($response -match "n|no") {
            Write-Host "Stopping"
            return
        } else {
            Write-Host "Cleaning"
        }
    }
    git clean -xfd -e packages -e .vs
    $cores = $env:NUMBER_OF_PROCESSORS
    msbuild /t:clean /m:$cores /verbosity:normal dirs.proj
    popd
    $outFolder = (Join-Path (Resolve-Path (Join-Path $PSScriptRoot "..\..\")) "out")
    if (Test-Path $outFolder) {
        pushd $outFolder 
        git clean -xfd
        popd
    }
}

<#
.SYNOPSIS
For NuGet packages that VS does not understand, use this script to automate modifying
all the places in the repo that need modification
#>
function Update-NugetPackage()
{
    param(
        $Package,
        $FromVersion,
        $ToVersion
    )
    $repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\"))
    pushd $repoRoot

    Write-Host "Modifying 'packages.config'"
    foreach ($packagesConfig in (Get-ChildItem -Filter packages.config -Recurse  -Exclude out/**/*,packages/**/*,obj/**/*)) {
        [xml]$parsed = (Get-Content $packagesConfig.FullName)
        foreach ($p in $parsed.packages.package) {
            if (($p.id -eq $package) -and ($p.version -eq $FromVersion)) {
                Write-Host "Modified '$($packagesConfig.FullName)'"
                $p.version = $ToVersion
                $parsed.Save($packagesConfig.FullName)
            }
        }
    }

    cd src
    Write-Host "Modifying relative paths in project files"
    foreach ($project in (Get-ChildItem -Filter *.*proj -Recurse -Exclude build\**\*,out\**\*,packages\**\*,obj\**\*,objd\**\*)) {
        $contents = Get-Content $project.FullName -Raw
        if ($contents.Contains("$($Package).$($FromVersion)")) {
            Write-Host "Modified '$($project.FullName)'"
            $newContent = $contents.Replace("$($Package).$($FromVersion)", "$($Package).$($ToVersion)") > $project.FullName
        }
    }
    popd
}