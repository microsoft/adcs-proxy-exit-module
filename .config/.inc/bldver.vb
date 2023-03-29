imports System.Reflection
imports System.Runtime.InteropServices

'
' if you want to use a private version file and customize this, see
' file://samsndrop02/CoreXT-Latest/docs/corext/corext/version.htm
'

<assembly: AssemblyVersion("1.1.0.343")>

<assembly: AssemblyCompany("Microsoft Corp")>
<assembly: AssemblyProduct("Microsoft Account")>
<assembly: AssemblyCopyright("2020")>


#if ENABLE_CODESIGN

#if ENABLE_PRS_DELAYSIGN
<assembly: AssemblyKeyFile("")>
<assembly: AssemblyKeyName("")>
<assembly: AssemblyDelaySign(true)>
#else
<assembly: AssemblyKeyFile("")>
<assembly: AssemblyKeyName("")>
<assembly: AssemblyDelaySign(false)>
#end if

#end if

