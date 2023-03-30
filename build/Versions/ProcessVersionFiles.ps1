
    param(    
    [String] $Version,
    [String] $COREXTBRANCH,
    [String] $SourceFolder,
    [String] $DestinationFolder
 )

    $tokens = $Version.Split(".");
    $ProductMajor = $tokens[0];
    $ProductMinor = $tokens[1];
    $BuildMajor = $tokens[2];
    $BuildMinor = $tokens[3];
    
    $ProductMajorNumber = $ProductMajor
    $ProductMinorNumber = "0";
    
    $BuildMajorNumber = $BuildMajor
    if ($BuildMinor -eq "00")
    {
       $BuildMinorNumber = "0";
    }
    else
    {
       $BuildMinorNumber = $BuildMinor;
    }

    if (!(test-path "$DestinationFolder"))
    {
      New-Item -Path "$DestinationFolder" -ItemType Directory;
    }
    Write-Host "Processing  $SourceFolder to $DestinationFolder"

    $content = (Get-Content "$SourceFolder\bldver.hf").Replace("%ProductMajorNumber%",$ProductMajorNumber).Replace("%ProductMinorNumber%",$ProductMinorNumber).Replace("%BuildMajorNumber%",$BuildMajorNumber).Replace("%BuildMinorNumber%",$BuildMinorNumber).Replace("%ProductMajor%",$ProductMajor).Replace("%ProductMinor%",$ProductMinor).Replace("%BuildMajor%",$BuildMajor).Replace("%BuildMinor%",$BuildMinor).Replace("%USERNAME%", $env:USERNAME).Replace("%COMPUTERNAME%",$env:COMPUTERNAME).Replace("%corextBranch%", $COREXTBRANCH);
    Set-Content -Path "$DestinationFolder\bldver.h" $content -Force

    $content = (Get-Content "$SourceFolder\bldvercs.hf").Replace("%ProductMajor%",$ProductMajor).Replace("%ProductMinor%",$ProductMinor).Replace("%BuildMajor%",$BuildMajor).Replace("%BuildMinor%",$BuildMinor).Replace("%USERNAME%", $env:USERNAME).Replace("%CODESIGN_KEY%",$env:CODESIGN_KEY).Replace("%CODESIGN_TESTKEY%", $env:CODESIGN_TESTKEY);
    Set-Content -Path "$DestinationFolder\bldver.cs" $content -Force

    $content = (Get-Content "$SourceFolder\bldvervb.hf").Replace("%ProductMajor%",$ProductMajor).Replace("%ProductMinor%",$ProductMinor).Replace("%BuildMajor%",$BuildMajor).Replace("%BuildMinor%",$BuildMinor).Replace("%USERNAME%", $env:USERNAME).Replace("%CODESIGN_KEY%",$env:CODESIGN_KEY).Replace("%CODESIGN_TESTKEY%", $env:CODESIGN_TESTKEY);
    Set-Content -Path "$DestinationFolder\bldver.vb" $content -Force

    $content = (Get-Content "$SourceFolder\bldvermc.hf").Replace("%ProductMajor%",$ProductMajor).Replace("%ProductMinor%",$ProductMinor).Replace("%BuildMajor%",$BuildMajor).Replace("%BuildMinor%",$BuildMinor).Replace("%USERNAME%", $env:USERNAME).Replace("%CODESIGN_KEY%",$env:CODESIGN_KEY).Replace("%CODESIGN_TESTKEY%", $env:CODESIGN_TESTKEY);
    Set-Content -Path "$DestinationFolder\bldver.cpp" $content -Force

    $version_htm_content = "Version $ProductMajor.$ProductMinor (Build $BuildMajor.$BuildMinor)" 
    Set-Content -Path "$DestinationFolder\version.htm" $version_htm_content -Force

    # Needed for cloudbuild to parse version information.
    # See details at https://mseng.visualstudio.com/Domino/_git/CloudBuild?path=%2Fprivate%2FCommon%2FRpc%2Fsrc%2FBuildCommon%2FDrop%2FBranchVersion.cs&version=GBmaster
    $version_htm_in_public_dir = "$SourceFolder\..\..\public\inc\version.htm"
    if (-not(Test-Path $version_htm_in_public_dir))
    {
       New-Item -Path $version_htm_in_public_dir -ItemType File -Force
    }

    Set-Content -Path $version_htm_in_public_dir $version_htm_content -Force

     