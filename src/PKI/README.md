# PMI Exit Module
This is an Exit Module that runs inproc to the certificate authority service process (certsrv.exe). 

## Components
1. ExitModule.dll - COM objects of the PMICertExit class and PMIExitModule (UI) class. 
2. ExitModulePS.dll - Generated Proxy/stub DLL. Should be able to delete later.

## Registration
1. Each time, create a new folder for the build you are trying to register.
2. Use regsvr32 ExitModule.dll to register the COM objects and type library. Run regsvr32 /u on the old build, not the new build. 
3. Use MMC to select "PMI Exit Module" in the list of exit modules. This will cause the certificate services to restart. If patching an old build, restart the certificate authority service.
4. TODO: investigate a non-UI way to register.

## Design/Coding Decisions
This follows Windows Coding and Design Conventions.
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
