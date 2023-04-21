# PMI Exit Module
This is an Exit Module that runs inproc to the certificate authority service process (certsrv.exe). 

## Components
1. ExitModule.dll - COM objects of the PMICertExit class and PMIExitModule (UI) class. 
2. ExitModulePS.dll - Generated Proxy/stub DLL. Not in build. Should be able to delete later.

## Design
The exit module is a COM object that runs inside the certificate authority service and receives notifications when certs are issued. That component invokes a registered Event Processor EXE to do any further processing.
The event processor EXE can crash and be written in managed code vs. the COM exit module that needs to be native code that runs in-proc to a critical service and even needs to handle low memory conditions.
The exit module spawns the registered process and waits 10s for it to finish.
The exit module COM object also exposes something called an Exit Manage Module. This is a UI component that gets loaded in MMC to allow the admin to select the exit module.

### Arguments to the Event Processor EXE
    <event processor.exe> <operation> [Options]
    Operation:
        certissued [CertIssuedOptions]
    CertIssuedOptions:
        -subjectkeyidentifier "<value>" - Hex encoded subject key identifier with spaces between the bytes.
        -serialnumber <value> - The string for the serial number.
        -rawcertpath <path> - A path to the raw certificate data. This is a temp file that gets deleted when the process exits.

Assume new options an operations can get added in the future. Options will alway be -option [0..N args]. Look for - for the start of a new option. Ignore options that are not understood.
Assume the options can come in at any order, but the 1st arg is always the operation.
The event processor is expected to return an exit code of 0 to indicate success.
Return exit code 0 for unsupported operations.

### Launching PowerShell instead of a custom EXE
The Exit module will invoke PowerShell. To do this, update the ExePath to point to PowerShell.exe. There is a MULTI_SZ registry value for supplying static arguments ahead of the dynamic arguments provided by the exit module. The ExitModuleExe.reg
has already been updated as an example. SampleScript.ps1 is also checked in that shows how to declare the arguments in the script.

### Windows Events
The Exit module writes events to the Windows Application Log. 
There is an informational event when a process is launched showing the full command line for it along with process ID.
There is an informational event when the process exits with exit code 0 indicating success.
There is an warning message if the exit module times out wwaiting for the process to exit. It includes the path to the temp file for debugging.
There is an error event if the process could not be launched. The error code is included and the decoded error message.
There is an error event if the process exits with a code other than 0. It also includes the path to the temp file that gets preserved to allow debugging.

See registration section.

### Performance
It reads the registry for the path to the exe each time. This lets the event processor be registered w/o restarting the service. This should be ok until we need to do 1000+ certs/second.
The external process is launched for each cert. This should be ok given the volume.
TODO: Consider Win32 Jobs for the event processor.

### Security
TODO: Both the exit module and event processor need to be deployed to protected directories (like Program Files). Ideally, only spfcopy or trusted installer can update.
TODO: If the reg key for the event processor is not locked down (DACL), someone with lower priv can update and run their code as System.
TODO: The COM registration keys for the Exit module need to be locked down.

Extra Credit TODO: Only allow processes under %ProgramFiles%\PMI to get launched.

### Safe Deployment
Each build of the exit module should be copied to its own folder. This allows for rollback to an LKG build with regsvr32.
Each build of the event processor should be copied to its own folder. This allows rollback to an LKG build by editing the registry.
The exit module and the event processor can be shipped separately.
Build validation can be done by issuing benign certs and looking for the correct behavior.

## Registration
Follow the sub-sections in order.

### Event Message DLL registration (Each build)
TODO: This is clunky and needs a real setup.
1. Merge ExitModuleEventLog.reg and open regedit and fixup the path to PMIExitModuleMessages.dll
2. If you have EventViewer open, restart it. 

### COM Object Registration (Each build)
1. Each time you deploy, create a new folder for the build you are trying to register.
2. net stop CertSvc
3. Run regsvr32 /u on the old build, not the new build. 
4. Use regsvr32 ExitModule.dll to register the COM objects and type library. 
5. net start CertSvc

### One Time Registration
1. Use MMC to select "PMI Exit Module" in the list of exit modules. This will cause the certificate services to restart. If patching an old build, restart the certificate authority service.
2. TODO: investigate a non-UI way to register.

### Event Processor EXE Registration
1. Modify ExitModuleExe.reg to point to your EXE.
2. REG IMPORT ExitModuleExe.reg /r:64
3. Update values to point to your script your local path to powershell and any other static args.
3. No need to restart CertSvc

TODO: There is a bug where the code reading REG_EXPAND_SZ needs to expand environment vars.

## Debugging the Exit Module
Prereq: Learn WinDBG and gflags.

### Debugging certsrv.exe
To get debug trace and to debug initialization:
1. In the Services snap-in click the checkbox to enable desktop interaction and apply.
1. launch gflags.exe and go to the image tab.
2. Enter certsrv.exe and hit tab.
3. For the debugger, add '<path to windbg> -g -G -server npipe:pipe=MainPipe'
4. Restart CertSvc. Even if it doesn't start, you can go to the next step.
5. Launch Windbg and connect to a remote. The MainPipe should show up if you search on localhost.
6. You can see the trace output now and any exceptions. You can break in and debug what you want.

## Impl/Coding Decisions
This follows Windows Coding and Design Conventions.
Fun fact: certsrv.exe has minimal in-proc protections for components it loads in memory. That means an AV in your COM object will not crash the entire service, but could make it very unstable (like leaks and heap corruptions).
Fun fact: certsrv.exe runs as the System Account on the PKI server. Any security vulns lead to total compromise of the server. If that server is a domain controller, that means the whole domain.

1. ATL - The implemenation uses ATL. ATL provides a lot of boilerplate for doing COM objects and is still well-supported. No need to DIY.
2. C++ Exceptions - NOPE. This code runs in-proc to a Windows Service that is critical to the functioning of our cloud. This service must stay up even in low-memory conditions. C++ exceptions are not part of the COM contract, so each interface method would need try/catch logic. Also, if an exception cannot be handled (low-memory for example), the process will exit.
3. COM Smart Pointers - ATL::CComPtr<> vs. _com_ip_t. CComPtr does not throw exceptions. _com_ip_t does. CComPtr is the winner.
4. ATL::CComVariant vs. _variant_t. CComVariant does not throw. Use it.
5. ATL::ComBSTR vs. _bstr_t. CComBSTR does not throw. Use it.
6. STL - NOPE. STL throws exceptions. STL can be used for test projects and console apps, but not this exit module.
7. Any types from comip.h - NOPE. These all throw exceptions.
8. ::new() and ::new[](). The project is compiled to use the ones that do not throw.
9. goto statements - NOPE, not even for cleanup. Use break statements inside do { } while (false);
10. Early returns are ok if you guarantee cleanup happens. protip: Use classes with destructors.
11. Encapsulate memory/resource management in classes.
12. Use const& to pass non-null input to methods. Use const* for input that could be null. Use LPCWSTR for strings.
13. Use OUT CFoo& result for result objects. Use IN, INOUT, or REF CFoo& for non-const parameters.
14. Project uses the ISO C++14 standard.
15. Use nullptr C++ keyword vs. NULL.
16. Declare classes as non-copyable with the following syntax:
    private:
        CCertServerExit(const CCertServerExit&) = delete;
        CCertServerExit& operator=(const CCertServerExit&) = delete;
17. Abstract classes should declare a protected constructor.
18. Polymorphic classes must have a public virtual destructor on the base class.
19. Use Hungarian notation.
20. Use 4 spaces instead of a tab char.
21. One Parameter per line in method declarations.
22. For making calls into methods with a lot of weird parameters, comment which parameter is being passed ex.:
    HRESULT hr = m_ptrInner.CoCreateInstance(
        CLSID_CCertServerExit,
        nullptr, // pUnkOuter
        CLSCTX_INPROC_SERVER);
23. Don't use BSTRs internally. Only use them as needed for interfaces. Allocating them is super slow.

### Tracing/Logging
TODO. Tracing should be ETW. There was a handy thing called WPP in Windows to make this easy.

### Unit Tests
Yes. TODO later.


### File Header

/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        CertServerExit.cpp

    Abstract:

        Wrapper for ICertServerExit.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        10-Apr-2023 jrowlett Created.

--*/

### Class Doc Comment
/*++

    Abstract:

        Wrapper for ICertServerExit.

--*/
class CCertServerExit
{

### Method Doc Comment
    /*++
    
        Abstract:

            Initializes the server interface with an optional context.

        Parameters:

            lContext - the context passed to ICertExit::Notify() or 0.

        Returns:
            
            S_OK for success or an error code.

        Remarks:

            The context is optional during the ICertExit::Initialize() call.
    --*/
    HRESULT Init(
        LONG lContext = 0L);
