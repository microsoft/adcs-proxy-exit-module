/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        Process.cpp

    Abstract:

        CProcess class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        11-Apr-2023 jrowlett Created.

--*/
#include "pch.h"
#include "Process.h"

constexpr const size_t g_cchMaxCommandLine = 32768;

CProcess::CProcess()
{
    ZeroMemory(&m_stProcInfo, sizeof(m_stProcInfo));
    m_stProcInfo.hProcess = INVALID_HANDLE_VALUE;
    m_stProcInfo.hThread = INVALID_HANDLE_VALUE;
}

CProcess::~CProcess()
{
    if (m_stProcInfo.hProcess != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_stProcInfo.hProcess);
    }

    if (m_stProcInfo.hThread != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_stProcInfo.hThread);
    }
}

HRESULT CProcess::Create(
    LPCWSTR pwszApplicationName,
    const CBuffer<LPCWSTR>& bufArguments,
    DWORD dwCreationFlags)
{
    HRESULT hr = S_OK;
    STARTUPINFOW stStartupInfo;
    ZeroMemory(&stStartupInfo, sizeof(stStartupInfo));
    stStartupInfo.cb = sizeof(PROCESS_INFORMATION);
    CHeapBuffer<WCHAR> bufCmdLine;

    if (m_stProcInfo.hProcess != INVALID_HANDLE_VALUE)
    {
        ATLTRACE(L"Process already created, hProcess=%p\n", m_stProcInfo.hProcess);
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    }

    if (!bufCmdLine.Alloc(g_cchMaxCommandLine))
    {
        ATLTRACE(L"Failed to alloc buffer for command line.\n");
        return E_OUTOFMEMORY;
    }

    hr = FormatCommandLine(
        pwszApplicationName,
        bufArguments,
        OUT bufCmdLine);
    if (FAILED(hr))
    {
        ATLTRACE("CProcess::FormatCommandLine failed, hr=%x\n", hr);
        return hr;
    }

    ATLTRACE(
        L"Launching [%s] with [%s] command line.\n",
        pwszApplicationName,
        bufCmdLine.Get());
    if (!CreateProcessW(
        NULL, // lpApplicationName
        bufCmdLine.Get(), // lpCommandLine
        NULL, // lpProcessAttributes
        NULL, // lpThreadAttributes
        FALSE, // bInheritHandles
        dwCreationFlags, // dwCreationFlags
        NULL, // lpEnvironment
        NULL, // lpCurrentDirectory
        &stStartupInfo, // lpStartupInfo
        &m_stProcInfo)) // lpProcessInformation
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"CreateProcessW failed, hr=%x\n", hr);
        return hr;
    }

    ATLTRACE(
        L"Process created. ProcessID=%d, ThreadID=%d\n",
        m_stProcInfo.dwProcessId, 
        m_stProcInfo.dwThreadId);

    return hr;
}

HRESULT CProcess::Wait(
    DWORD dwMilliseconds)
{
    DWORD dwRes = ::WaitForSingleObject(
        m_stProcInfo.hProcess,
        dwMilliseconds);
    switch (dwRes)
    {
    case WAIT_OBJECT_0:
        return S_OK;
    case WAIT_ABANDONED:
        return E_ABORT;
    case WAIT_TIMEOUT:
        return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    case WAIT_FAILED:
        return HRESULT_FROM_WIN32(::GetLastError());
    }

    return E_UNEXPECTED;
}

HRESULT CProcess::GetExitCode(
    OUT DWORD& dwExitCode)
{
    if (!::GetExitCodeProcess(m_stProcInfo.hProcess, &dwExitCode))
    {
        return HRESULT_FROM_WIN32(::GetLastError());
    }

    return S_OK;
}

HRESULT CProcess::FormatCommandLine(
    LPCWSTR pwszApplicationName,
    const CBuffer<LPCWSTR>& bufArgs,
    OUT CBuffer<WCHAR>& bufResult)
{
    HRESULT hr = S_OK;
    LPWSTR pwszCurrent = bufResult.Get();
    size_t cchMax = bufResult.GetLength();

    hr = WriteArgument(
        pwszCurrent,
        cchMax,
        pwszApplicationName);
    if (FAILED(hr))
    {
        return hr;
    }

    // NOTE: Not using StringCchCat* because it repeats evaluation of string lengths.
    for (const LPCWSTR* p = bufArgs.Get();
        p != bufArgs.Get() + bufArgs.GetLength();
        p++)
    {
        LPCWSTR pwszArg = *p;

        // Add argument separator.
        hr = ::StringCchCopyExW(
            pwszCurrent,
            cchMax,
            L" ",
            &pwszCurrent,
            &cchMax,
            STRSAFE_IGNORE_NULLS);
        if (FAILED(hr))
        {
            break;
        }

        hr = WriteArgument(pwszCurrent, cchMax, pwszArg);
        if (FAILED(hr))
        {
            break;
        }
    }

    return S_OK;
}

bool CProcess::HasWhiteSpace(LPCWSTR pwsz)
{
    for (const WCHAR* p = pwsz;
        *p;
        p++)
    {
        if (*p == L' ')
        {
            return true;
        }
    }

    return false;
}

HRESULT CProcess::WriteArgument(
    LPWSTR& pwszCurrent,
    size_t& cchMax,
    LPCWSTR pwszArg)
{
    // NOTE: This isn't full feastured argument escaping. 
    // It handles the current use case of embedded spaces.
    // It does not handle embedded quotes.
    bool fEscape = HasWhiteSpace(pwszArg);
    HRESULT hr = S_OK;

    do
    {
        if (fEscape)
        {
            hr = ::StringCchCopyExW(
                pwszCurrent,
                cchMax,
                L"\"",
                &pwszCurrent,
                &cchMax,
                STRSAFE_IGNORE_NULLS);
            if (FAILED(hr))
            {
                break;
            }
        }

        hr = ::StringCchCopyExW(
            pwszCurrent,
            cchMax,
            pwszArg,
            &pwszCurrent,
            &cchMax,
            STRSAFE_IGNORE_NULLS);
        if (FAILED(hr))
        {
            break;
        }

        if (fEscape)
        {
            hr = ::StringCchCopyExW(
                pwszCurrent,
                cchMax,
                L"\"",
                &pwszCurrent,
                &cchMax,
                STRSAFE_IGNORE_NULLS);
            if (FAILED(hr))
            {
                break;
            }
        }
    } while (false);

    return hr;
}