#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        PMIExitModuleEventSource.h

    Abstract:

        CPMIExitModuleEventSource class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        18-Apr-2023 jrowlett Created.

--*/

#include "EventSource.h"

/*++

    Abstract:

        Event Source for this module.

--*/
class CPMIExitModuleEventSource : public CEventSource
{
public:
    inline CPMIExitModuleEventSource()
        : CEventSource(s_pwszProviderName)
    {
    }

    inline ~CPMIExitModuleEventSource()
    {
    }

    /*++
    
        Abstract:

            Reports a message with text similar to:
            The process [%1] with main thread id [%2] succeeded with exit code [%3].

        Parameters:

            dwProcessID - process ID.
            dwThreadID - main thread ID.
            dwExitCode - exit code from the process.

    --*/
    void ReportProcessSucceeded(
        DWORD dwProcessID,
        DWORD dwThreadID,
        DWORD dwExitCode) const;

    /*++

        Abstract:

            Reports a message with text similar to:
            The process [%1] with main thread id [%2] failed with exit code [%3]. The temp file [%4] will be preserved for debugging.

        Parameters:
        
            dwProcessID - process id.
            dwThreadID - thread id.
            dwExitCode - process exit code.
            pwszTempFilePath - path to the temp file that will not be deleted.

    --*/
    void ReportProcessFailed(
        DWORD dwProcessID,
        DWORD dwThreadID,
        DWORD dwExitCode,
        LPCWSTR pwszTempFilePath) const;

    /*++

        Abstract:

            Reports a message with text similar to:
            Timed out waiting %1 seconds for process [%2] with main thread id [%3]. The process was not killed and could still be running. The temp file [%4] will be preserved for debugging.

        Parameters:

            dwSeconds - the timeout in seconds.
            dwProcessID - the process id.
            dwThreadID - the thread id.
            pwszTempFilePath - path to the temp file that will not get deleted.

    --*/
    void ReportProcessTimedOut(
        DWORD dwSeconds,
        DWORD dwProcessID,
        DWORD dwThreadID,
        LPCWSTR pwszTempFilePath) const;

    /*++

        Abstract:

            Reports a message with text similar to:
            Started process [%1] with the following command line [%2]. Process ID=[%3]. Thread ID=[%4].

        Parameters:

            pwszExePath - path to the exe that was started.
            pwszCmdLine - full command line.
            dwProcessID - process ID of the started process.
            dwThreadID - main thread ID of the started process.

    --*/
    void ReportProcessStartSucceeded(
        LPCWSTR pwszExePath,
        LPCWSTR pwszCmdLine,
        DWORD dwProcessID,
        DWORD dwThreadID) const;

    /*++

        Abstract:

            Reports a message with text similar to:
            Failed to start process [%1] with the following command line [%2]. HRESULT=%3. %4

        Parameters:

            pwszExePath - Path to the exe that was attempted.
            pwszCmdLine - full command line.
            hrError - error code.

        Remarks:

            The last message string is the error message text for the given HRESULT.
    --*/
    void ReportProcessStartFailed(
        LPCWSTR pwszExePath,
        LPCWSTR pwszCmdLine, 
        HRESULT hrError) const;

private:
    static const LPCWSTR s_pwszProviderName;
};

