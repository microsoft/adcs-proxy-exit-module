#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        Process.h

    Abstract:

        CProcess class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        11-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:

        Win32 Process wrapper.
--*/
class CProcess
{
public:
    CProcess();
    ~CProcess();

    /*++
    
        Abstract:

            Gets the process ID.

        Returns:

            the process id.
    --*/
    inline DWORD GetProcessID() const
    {
        return m_stProcInfo.dwProcessId;
    }

    /*++

        Abstract:

            Gets the thread ID.

        Returns:

            the thread id.
    --*/
    inline DWORD GetThreadID() const
    {
        return m_stProcInfo.dwThreadId;
    }

    /*++
    
        Abstract:

            Creates the process.

        Parameters:

            pwszApplicationName - path to exe.
            bufArguments - array of args.
            dwCreationFlags - flags to pass to CreateProcessW.

        Returns:

            S_OK - success.
            other - error code.
    --*/
    HRESULT Create(
        LPCWSTR pwszApplicationName,
        const CBuffer<LPCWSTR>& bufArguments,
        DWORD dwCreationFlags);

    /*++
    
        Abstract:

            Waits for the process to exit.

        Parameters:

            dwMilliseconds - the number of milliseconds to wait.

        Returns:

            S_OK - the process exited.
            E_ABANDONED - The wait was abandoned.
            E_TIMEOUT - the wait timed out.
            Other - error code.
    --*/
    HRESULT Wait(
        DWORD dwMilliseconds);

    /*++
    
        Abstract:

            Gets the exit code from the process.

        Parameters:

            dwExitCode - on success, receives the exit code.

        Returns:

            S_OK - success.
            Other - error code.
    --*/
    HRESULT GetExitCode(
        OUT DWORD& dwExitCode);

private:
    PROCESS_INFORMATION m_stProcInfo;

    static HRESULT FormatCommandLine(
        LPCWSTR pwszApplicationName,
        const CBuffer<LPCWSTR>& args,
        OUT CBuffer<WCHAR>& bufResult);

    static bool HasWhiteSpace(LPCWSTR pwsz);
    static HRESULT WriteArgument(
        LPWSTR& pwszCurrent,
        size_t& cchMax,
        LPCWSTR pwszArg);

    CProcess(const CProcess&) = delete;
    CProcess& operator=(const CProcess&) = delete;
};

