/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        main.cpp

    Abstract:

        Main entry point.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        24-Apr-2023 jrowlett Created.

--*/
#include <iostream>
#include "Arguments.h"
#include <windows.h>
#include <winreg.h>
#include <shlwapi.h>
#include "../PMIExitModuleMessages/PMIExitModuleEventSourceDef.h"

#define APPLICATION_PATH L"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application"
constexpr const wchar_t* g_pwszMessageDLLName = L"PMIExitModuleMessages.dll";
constexpr const wchar_t* g_pwszRegKeyPath = APPLICATION_PATH L"\\" WSZ_PMIEXITMODULE_PROVIDERNAME;

void PrintUsage();
bool InstallMessageDLL(const std::wstring& strPath);
bool UninstallMessageDLL();
bool FindMessageDLL(OUT std::wstring& strPath);

/*++

    Abstract:

        Main entry point.

    Arguments:

        argc - count of program arguments.
        argv - array of program arguments.

    Returns:

        0 - success.
        1 - error.

    Remarks:

        Updates the registry to point to the message dll.
        Usage:

            PMIExitModuleMessages.exe /?
                prints usage.
            PMIExitModuleMessages.exe [/i] [path to PMIExitModuleMessages.dll]
                installs the messages. If the path is not specified, it looks at the path where this program is.
            PMIExitModuleMessages.exe /u
                uninstalls the dll.

--*/
int __cdecl wmain(
    int argc,
    const wchar_t* argv[])
{
    CArguments args;
    if (!args.TryParse(argc, argv))
    {
        PrintUsage();
        return EXIT_FAILURE;
    }

    bool fSuccess = false;
    switch (args.GetOperation())
    {
        case Operation::INSTALL:
        {
            std::wstring strPath = args.GetPath();
            fSuccess = true;
            if (strPath.empty())
            {
                fSuccess = FindMessageDLL(strPath);
            }

            if (fSuccess)
            {
                fSuccess = InstallMessageDLL(strPath);
            }
        } break;
        case Operation::UINSTALL:
        {
            fSuccess = UninstallMessageDLL();
        } break;
    }

    return fSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}

void PrintUsage()
{
    std::wcerr << L"Usage:" << std::endl;
    std::wcerr << L"PMIExitModuleMessages.exe /?" << std::endl;
    std::wcerr << L"    prints usage." << std::endl;
    std::wcerr << L"PMIExitModuleMessages.exe [/i] [path to PMIExitModuleMessages.dll]" << std::endl;
    std::wcerr << L"    installs the messages. If the path is not specified, it looks at the path where this program is." << std::endl;
    std::wcerr << L"PMIExitModuleMessages.exe /u" << std::endl;
    std::wcerr << L"    uninstalls the messages dll." << std::endl;
}

bool InstallMessageDLL(const std::wstring& strPath)
{
    HKEY hkey = NULL;
    LSTATUS lStatus = ::RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,
        g_pwszRegKeyPath,
        0, // Reserved
        NULL, // lpClass
        REG_OPTION_NON_VOLATILE,
        KEY_WOW64_64KEY | KEY_READ | KEY_WRITE,
        NULL, // lpSecurityAttributes
        &hkey,
        NULL); // lpdwDisposition
    if (lStatus != ERROR_SUCCESS)
    {
        std::wcerr << L"RegCreateKeyExW failed, Win32 error code=" << lStatus << std::endl;
        return false;
    }

    DWORD dwValue = PMI_EXITMODULE_CATEGORYCOUNT;
    lStatus = ::RegSetKeyValueW(
        hkey,
        NULL,
        L"CategoryCount",
        REG_DWORD,
        &dwValue, 
        sizeof(dwValue));
    if (lStatus != ERROR_SUCCESS)
    {
        std::wcerr << L"RegSetKeyValueW(CategoryCount) failed, Win32 error code=" << lStatus << std::endl;
        return false;
    }

    dwValue = PMI_EXITMODULE_PARAMETERCOUNT;
    lStatus = ::RegSetKeyValueW(
        hkey,
        NULL,
        L"ParameterCount",
        REG_DWORD,
        &dwValue,
        sizeof(dwValue));
    if (lStatus != ERROR_SUCCESS)
    {
        std::wcerr << L"RegSetKeyValueW(ParameterCount) failed, Win32 error code=" << lStatus << std::endl;
        return false;
    }

    static const LPCWSTR rgpwszPathValueNames[] =
    {
        L"CategoryMessageFile",
        L"EventMessageFile",
        L"ParameterMessageFile",
    };

    LPCWSTR pwszPath = strPath.c_str();
    DWORD cbPath = (DWORD)strPath.length() * sizeof(WCHAR);
    for (
        const LPCWSTR* ppwszName = rgpwszPathValueNames;
        ppwszName != rgpwszPathValueNames + sizeof(rgpwszPathValueNames) / sizeof(rgpwszPathValueNames[0]);
        ppwszName++)
    {
        LPCWSTR pwszName = *ppwszName;
        lStatus = ::RegSetKeyValueW(
            hkey,
            NULL,
            pwszName,
            REG_SZ,
            pwszPath,
            cbPath);
        if (lStatus != ERROR_SUCCESS)
        {
            std::wcerr << L"RegSetKeyValueW(" << pwszName << L") failed, Win32 error code = " << lStatus << std::endl;
            return false;
        }
    }

    return true;
}

bool UninstallMessageDLL()
{
    LSTATUS lStatus = ::RegDeleteKeyExW(
        HKEY_LOCAL_MACHINE,
        g_pwszRegKeyPath,
        KEY_WOW64_64KEY | DELETE,
        0); // Reserved
    if (lStatus == ERROR_SUCCESS || lStatus == ERROR_FILE_NOT_FOUND)
    {
        return true;
    }

    std::wcerr << L"Failed to delete reg key HKLM:\\" << g_pwszRegKeyPath << L", Win32 error code=" << lStatus << std::endl;
    return false;
}

bool FindMessageDLL(OUT std::wstring& strPath)
{
    LSTATUS lStatus = ERROR_SUCCESS;
    WCHAR wszPath[MAX_PATH + 1];
    DWORD cch = ::GetModuleFileNameW(
        NULL,
        wszPath,
        sizeof(wszPath) / sizeof(wszPath[0]));
    if (cch == 0)
    {
        lStatus = ::GetLastError();
        std::wcerr << L"GetModuleFileNameW failed, Win32 error code=" << lStatus << std::endl;
        return false;
    }

    const wchar_t* pchDirEnd = wcsrchr(wszPath, L'\\');
    if (!pchDirEnd)
    {
        std::cerr << L"Unable to look for the message DLL based on the location of the current process: " << wszPath << std::endl;
        return false;
    }

    strPath = std::wstring(wszPath, pchDirEnd - wszPath + 1);
    strPath += g_pwszMessageDLLName;

    if (!::PathFileExistsW(strPath.c_str()))
    {
        std::wcerr << L"Message DLL not found at path " << strPath << std::endl;
        return false;
    }

    return true;
}