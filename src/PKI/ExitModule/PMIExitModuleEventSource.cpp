/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        PMIExitModuleEventSource.cpp

    Abstract:

        CPMIExitModuleEventSource class impl.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        18-Apr-2023 jrowlett Created.

--*/

#include "pch.h"
#include "PMIExitModuleEventSource.h"
#include <PMIExitModuleMessages.h>

const LPCWSTR CPMIExitModuleEventSource::s_pwszProviderName = L"PMIExitModule";

void CPMIExitModuleEventSource::ReportProcessSucceeded(
    DWORD dwProcessID,
    DWORD dwThreadID,
    DWORD dwExitCode) const
{
    CNumericEventArg<DWORD> argProcessID(dwProcessID);
    CNumericEventArg<DWORD> argThreadID(dwThreadID);
    CNumericEventArg<DWORD> argExitCode(dwExitCode);
    CEventArg* rgArgs[] =
    {
        &argProcessID,
        &argThreadID,
        &argExitCode,
    };

    CRefBuffer<CEventArg*> bufArgs(rgArgs, sizeof(rgArgs) / sizeof(rgArgs[0]));
    HRESULT hr = ReportEvent(
        EVENTLOG_INFORMATION_TYPE,
        GENERAL_CATEGORY,
        MSG_PROCESS_SUCCEEDED,
        bufArgs,
        CRefBuffer<BYTE>());
    if (FAILED(hr))
    {
        ATLTRACE(L"ReportProcessSucceeded failed, hr=%x\n", hr);
    }
}

void CPMIExitModuleEventSource::ReportProcessFailed(
    DWORD dwProcessID,
    DWORD dwThreadID,
    DWORD dwExitCode,
    LPCWSTR pwszTempFilePath) const
{
    CNumericEventArg<DWORD> argProcessID(dwProcessID);
    CNumericEventArg<DWORD> argThreadID(dwThreadID);
    CNumericEventArg<DWORD> argExitCode(dwExitCode);
    CStringEventArg argTempFile(pwszTempFilePath);
    CEventArg* rgArgs[] =
    {
        &argProcessID,
        &argThreadID,
        &argExitCode,
        &argTempFile,
    };

    CRefBuffer<CEventArg*> bufArgs(rgArgs, sizeof(rgArgs) / sizeof(rgArgs[0]));
    HRESULT hr = ReportEvent(
        EVENTLOG_ERROR_TYPE,
        GENERAL_CATEGORY,
        MSG_PROCESS_FAILED,
        bufArgs,
        CRefBuffer<BYTE>());
    if (FAILED(hr))
    {
        ATLTRACE(L"ReportProcessFailed failed, hr=%x\n", hr);
    }
}

void CPMIExitModuleEventSource::ReportProcessTimedOut(
    DWORD dwSeconds,
    DWORD dwProcessID,
    DWORD dwThreadID,
    LPCWSTR pwszTempFilePath) const
{
    CNumericEventArg<DWORD> argSeconds(dwSeconds);
    CNumericEventArg<DWORD> argProcessID(dwProcessID);
    CNumericEventArg<DWORD> argThreadID(dwThreadID);
    CStringEventArg argTempFile(pwszTempFilePath);
    CEventArg* rgArgs[] =
    {
        &argSeconds,
        &argProcessID,
        &argThreadID,
        &argTempFile,
    };

    CRefBuffer<CEventArg*> bufArgs(rgArgs, sizeof(rgArgs) / sizeof(rgArgs[0]));
    HRESULT hr = ReportEvent(
        EVENTLOG_WARNING_TYPE,
        GENERAL_CATEGORY,
        MSG_PROCESS_TIMEDOUT,
        bufArgs,
        CRefBuffer<BYTE>());
    if (FAILED(hr))
    {
        ATLTRACE(L"ReportProcessTimedOut failed, hr=%x\n", hr);
    }
}

void CPMIExitModuleEventSource::ReportProcessStartSucceeded(
    LPCWSTR pwszExePath,
    LPCWSTR pwszCmdLine,
    DWORD dwProcessID,
    DWORD dwThreadID) const
{
    CStringEventArg argExePath(pwszExePath);
    CStringEventArg argCmdLine(pwszCmdLine);
    CNumericEventArg<DWORD> argProcessID(dwProcessID);
    CNumericEventArg<DWORD> argThreadID(dwThreadID);

    CEventArg* rgArgs[] =
    {
        &argExePath,
        &argCmdLine,
        &argProcessID,
        &argThreadID,
    };

    CRefBuffer<CEventArg*> bufArgs(rgArgs, sizeof(rgArgs) / sizeof(rgArgs[0]));
    HRESULT hr = ReportEvent(
        EVENTLOG_INFORMATION_TYPE,
        GENERAL_CATEGORY,
        MSG_PROCESS_START_SUCCEEDED,
        bufArgs,
        CRefBuffer<BYTE>());
    if (FAILED(hr))
    {
        ATLTRACE(L"ReportProcessStartSucceeded failed, hr=%x\n", hr);
    }
}

void CPMIExitModuleEventSource::ReportProcessStartFailed(
    LPCWSTR pwszExePath,
    LPCWSTR pwszCmdLine,
    HRESULT hrError) const
{
    CStringEventArg argExePath(pwszExePath);
    CStringEventArg argCmdLine(pwszCmdLine);
    CNumericEventArg<HRESULT> argError(hrError);
    CErrorMessageEventArg argErrorMessage(hrError);

    CEventArg* rgArgs[] =
    {
        &argExePath,
        &argCmdLine,
        &argError,
        &argErrorMessage,
    };

    CRefBuffer<CEventArg*> bufArgs(rgArgs, sizeof(rgArgs) / sizeof(rgArgs[0]));
    HRESULT hr = ReportEvent(
        EVENTLOG_ERROR_TYPE,
        GENERAL_CATEGORY,
        MSG_PROCESS_START_FAILED,
        bufArgs,
        CRefBuffer<BYTE>());
    if (FAILED(hr))
    {
        ATLTRACE(L"ReportProcessStartFailed failed, hr=%x\n", hr);
    }
}