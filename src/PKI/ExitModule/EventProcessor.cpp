/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventProcessor.cpp

    Abstract:

        CEventProcessor class impl.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        11-Apr-2023 jrowlett Created.

--*/

#include "pch.h"
#include "EventProcessor.h"
#include "TempFile.h"
#include "Process.h"

LPCWSTR g_pwszRegSubkey = L"Software\\Microsoft\\PMI\\PMIExitModule";
LPCWSTR g_pwszExePathValueName = L"ExePath";
LPCWSTR g_pwszTempFileNamePrefix = L"PMI";
constexpr const DWORD g_dwProcessTimeoutMSecs = 10000;
constexpr const size_t g_cbRegValueBuffer = 1024;

CEventProcessor::CEventProcessor()
{
}

CEventProcessor::~CEventProcessor()
{
}

HRESULT CEventProcessor::Init()
{
    HRESULT hr = S_OK;
    LSTATUS lr = ERROR_SUCCESS;
    HKEY hkeyModule = nullptr;
    DWORD dwType = 0;
    DWORD cbBuf = 0;

    do
    {
        lr = ::RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            g_pwszRegSubkey,
            0,              // dwReserved
            KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE,
            &hkeyModule);
        if (lr != ERROR_SUCCESS)
        {
            hr = HRESULT_FROM_WIN32(lr);
            ATLTRACE(L"Failed to open reg key HKLM\\%s, hr=%x\n", g_pwszRegSubkey, hr);
            break;
        }

        if (!m_strExePath.Alloc(g_cbRegValueBuffer))
        {
            ATLTRACE(L"Failed to alloc wchars for exe path.\n");
            hr = E_OUTOFMEMORY;
            break;
        }
        
        cbBuf = (DWORD)m_strExePath.GetSize();
        lr = ::RegQueryValueExW(
            hkeyModule, 
            g_pwszExePathValueName, 
            NULL,           // lpdwReserved
            &dwType,
            (BYTE*)m_strExePath.Get(),
            &cbBuf);
        if (lr != ERROR_SUCCESS)
        {
            hr = HRESULT_FROM_WIN32(lr);
            ATLTRACE(L"Failed to query reg value %s, hr=%x\n", g_pwszExePathValueName, hr);
            break;
        }

        if (dwType != REG_SZ)
        {
            ATLTRACE(L"Expected REG_SZ, actual %d\n", dwType);
            hr = E_UNEXPECTED;
            break;
        }
    } while (false);

    if (hkeyModule)
    {
        ::RegCloseKey(hkeyModule);
    }

    return hr;
}

HRESULT CEventProcessor::GetTempFilePath(
    OUT CHeapWString& strPath)
{
    HRESULT hr = S_OK;
    CStaticBuffer<WCHAR, MAX_PATH + 1> strFolder;

    DWORD cch = ::GetTempPathW((DWORD)strFolder.GetLength(), strFolder.Get());
    if (cch == 0)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"::GetTempPathW failed, hr=%x\n", hr);
        return hr;
    }

    if (!strPath.Alloc(MAX_PATH + 1))
    {
        hr = E_OUTOFMEMORY;
        ATLTRACE(L"Failed to alloc MAX_PATH+1.\n");
        return hr;
    }

    UINT uRes = ::GetTempFileNameW(
        strFolder.Get(),
        g_pwszTempFileNamePrefix,
        0, // uUnique
        strPath.Get());
    if (uRes == 0)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE("::GetTempFileNameW failed, hr=%x\n", hr);
    }

    return hr;
}

HRESULT CEventProcessor::NotifyCertIssued(
    LPCWSTR pwszSubjectKeyIdentifier,
    LPCWSTR pwszSerialNumber,
    const CBuffer<BYTE>& bufRawCert) const
{
    CHeapWString strTempFile;
    CTempFile objTempFile;

    HRESULT hr = GetTempFilePath(strTempFile);
    if (FAILED(hr))
    {
        ATLTRACE(L"GetTempFilePath failed, hr=%x\n", hr);
        return hr;
    }

    ATLTRACE(L"Writing cert to [%s]\n", strTempFile.Get());
    hr = objTempFile.Create(strTempFile.Get());
    if (FAILED(hr))
    {
        ATLTRACE(L"Creating temp file failed, hr=%x\n", hr);
        return hr;
    }

    size_t cbTotal = 0;
    while (cbTotal < bufRawCert.GetLength())
    {
        size_t cbWritten = 0;
        hr = objTempFile.Write(bufRawCert, 0, bufRawCert.GetLength(), OUT cbWritten);
        if (FAILED(hr))
        {
            ATLTRACE(L"Failed to write to temp file, hr=%x\n", hr);
            return hr;
        }

        cbTotal += cbWritten;
    }

    objTempFile.Close();

    LPCWSTR rgpwszArgs[] =
    {
        L"certissued",
        L"-subjectkeyidentifier",
        pwszSubjectKeyIdentifier,
        L"-serialnumber",
        pwszSerialNumber,
        L"-rawcertpath",
        strTempFile.Get()
    };

    CRefBuffer<LPCWSTR> bufArgs(rgpwszArgs, sizeof(rgpwszArgs) / sizeof(rgpwszArgs[0]));
    DWORD dwExitCode = 0;
    hr = RunProcess(bufArgs, OUT dwExitCode);
    if (FAILED(hr) || dwExitCode != 0)
    {
        if (FAILED(hr))
        {
            ATLTRACE(L"RunProcess failed, hr=%x\n");
        }

        ATLTRACE(
            L"Preserving temp file [%s] for debugging.\n",
            strTempFile.Get());
        objTempFile.Preserve();
    }

    return hr;
}

HRESULT CEventProcessor::RunProcess(
    const CBuffer<LPCWSTR>& bufArgs,
    OUT DWORD& dwExitCode) const
{
    HRESULT hr = S_OK;
    CProcess objProc;

    if (m_strExePath.GetLength() == 0)
    {
        ATLTRACE(L"No Process registered.\n");
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    }

    hr = objProc.Create(
        m_strExePath.Get(),
        bufArgs,
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP);
    if (FAILED(hr))
    {
        ATLTRACE(L"CProcess::Create failed, hr=%x\n", hr);
        return hr;
    }

    hr = objProc.Wait(g_dwProcessTimeoutMSecs);
    if (FAILED(hr))
    {
        ATLTRACE(L"CProcess::Wait failed, hr=%x\n", hr);
        return hr;
    }

    hr = objProc.GetExitCode(OUT dwExitCode);
    if (FAILED(hr))
    {
        ATLTRACE(L"CProcessL::GetExitCode failed, hr=%x\n", hr);
        return hr;
    }

    ATLTRACE(
        L"Process ID=%d with thread ID=%d exited with code=%d\n",
        objProc.GetProcessID(),
        objProc.GetThreadID(),
        dwExitCode);

    return hr;
}
