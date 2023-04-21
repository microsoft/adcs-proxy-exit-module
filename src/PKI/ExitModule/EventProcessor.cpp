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
#include "PMIExitModuleEventSource.h"
#include "EventProcessor.h"
#include "TempFile.h"
#include "Process.h"

LPCWSTR g_pwszTempFileNamePrefix = L"PMI";
constexpr const DWORD g_dwProcessTimeoutMSecs = 10000;

CEventProcessor::CEventProcessor(const CPMIExitModuleEventSource& objEventSource)
    : m_objEventSource(objEventSource)
{
}

CEventProcessor::~CEventProcessor()
{
}

HRESULT CEventProcessor::Init()
{
    return m_objConfig.Init();
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
    CHeapWString strEscSubjectKeyIdentifier;
    CHeapWString strEscTempFile;

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

    LPCWSTR pwszEscSubjectKeyIdentifier = pwszSubjectKeyIdentifier;
    LPCWSTR pwszEscTempFile = strTempFile.Get();
    if (m_objConfig.GetEscapeForPS())
    {
        hr = EscapeArgumentForPS(pwszSubjectKeyIdentifier, strEscSubjectKeyIdentifier);
        if (FAILED(hr))
        {
            ATLTRACE(L"Failed to escape subject key identifier, hr=%x\n", hr);
            return hr;
        }

        pwszEscSubjectKeyIdentifier = strEscSubjectKeyIdentifier.Get();

        hr = EscapeArgumentForPS(strTempFile.Get(), strEscTempFile);
        if (FAILED(hr))
        {
            ATLTRACE(L"Failed to escape temp file path, hr=%x\n", hr);
            return hr;
        }

        pwszEscTempFile = strEscTempFile.Get();
    }

    LPCWSTR rgpwszArgs[] =
    {
        L"certissued",
        L"-subjectkeyidentifier",
        pwszEscSubjectKeyIdentifier,
        L"-serialnumber",
        pwszSerialNumber,
        L"-rawcertpath",
        pwszEscTempFile,
    };

    constexpr size_t cArgs = sizeof(rgpwszArgs) / sizeof(rgpwszArgs[0]);
    const CBuffer<LPCWSTR>& bufBaseArgs = m_objConfig.GetArguments();
    CHeapBuffer<LPCWSTR> bufArgs;
    if (!bufArgs.Alloc(cArgs + bufBaseArgs.GetLength()))
    {
        hr = E_OUTOFMEMORY;
        ATLTRACE(L"Failed to alloc buffer for args.\n");
        return hr;
    }

    CopyMemory(bufArgs.Get(), bufBaseArgs.Get(), bufBaseArgs.GetSize());
    CopyMemory(bufArgs.Get() + bufBaseArgs.GetLength(), rgpwszArgs, cArgs * sizeof(LPCWSTR));

    DWORD dwExitCode = 0;
    hr = RunProcess(bufArgs, strTempFile.Get(), OUT dwExitCode);
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
    LPCWSTR pwszTempFile,
    OUT DWORD& dwExitCode) const
{
    HRESULT hr = S_OK;
    CProcess objProc;

    if (!m_objConfig.GetExePath())
    {
        ATLTRACE(L"No Process registered.\n");
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    }

    hr = objProc.Create(
        m_objConfig.GetExePath(),
        bufArgs,
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP);
    if (FAILED(hr))
    {
        ATLTRACE(L"CProcess::Create failed, hr=%x\n", hr);
        m_objEventSource.ReportProcessStartFailed(
            m_objConfig.GetExePath(),
            objProc.GetCommandLine(),
            hr);
        return hr;
    }

    m_objEventSource.ReportProcessStartSucceeded(
        m_objConfig.GetExePath(),
        objProc.GetCommandLine(),
        objProc.GetProcessID(),
        objProc.GetThreadID());

    hr = objProc.Wait(g_dwProcessTimeoutMSecs);
    if (FAILED(hr))
    {
        ATLTRACE(L"CProcess::Wait failed, hr=%x\n", hr);
        if (hr == HRESULT_FROM_WIN32(ERROR_TIMEOUT))
        {
            m_objEventSource.ReportProcessTimedOut(
                g_dwProcessTimeoutMSecs / 1000,
                objProc.GetProcessID(),
                objProc.GetThreadID(),
                pwszTempFile);
        }

        return hr;
    }

    hr = objProc.GetExitCode(OUT dwExitCode);
    if (FAILED(hr))
    {
        ATLTRACE(L"CProcess::GetExitCode failed, hr=%x\n", hr);
        return hr;
    }

    ATLTRACE(
        L"Process ID=%d with thread ID=%d exited with code=%d\n",
        objProc.GetProcessID(),
        objProc.GetThreadID(),
        dwExitCode);
    if (dwExitCode != 0)
    {
        m_objEventSource.ReportProcessFailed(
            objProc.GetProcessID(),
            objProc.GetThreadID(),
            dwExitCode,
            pwszTempFile);
    }
    else
    {
        m_objEventSource.ReportProcessSucceeded(
            objProc.GetProcessID(),
            objProc.GetThreadID(),
            dwExitCode);
    }

    return hr;
}

HRESULT CEventProcessor::EscapeArgumentForPS(
    LPCWSTR pwsz,
    OUT CHeapWString& strResult)
{
    HRESULT hr = S_OK;

    size_t cch = wcslen(pwsz) + 3;

    if (!strResult.Alloc(cch))
    {
        return E_OUTOFMEMORY;
    }

    LPWSTR pwszResult = strResult.Get();

    hr = ::StringCchCopyExW(
        pwszResult,
        cch,
        L"'",
        &pwszResult,
        &cch,
        STRSAFE_IGNORE_NULLS);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = ::StringCchCopyExW(
        pwszResult,
        cch,
        pwsz,
        &pwszResult,
        &cch,
        STRSAFE_IGNORE_NULLS);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = ::StringCchCopyExW(
        pwszResult,
        cch,
        L"'",
        &pwszResult,
        &cch,
        STRSAFE_IGNORE_NULLS);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}
