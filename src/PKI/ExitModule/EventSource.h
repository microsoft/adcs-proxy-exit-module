#pragma once
#include "EventArg.h"
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventSource.h

    Abstract:

        CEventSource class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        18-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:

        Wraps Win32 APIs for interacting with an event source.
--*/
class CEventSource
{
public:
    CEventSource(LPCWSTR pwszProviderName);
    ~CEventSource();

    /*++
    
        Abstract:

            Opens the event source by calling the Win32 RegisterEventSource API.

        Returns:
            S_OK - success.
            other - error.
    --*/
    HRESULT Open();

    /*++
    
        Abstract:

            Closes the event log.

    --*/
    void Close();

    /*++
    
        Abstract:

            Wrapper on the Win32 ReportEvent API.

        Arguments:
            
            wType - event type.
            wCategory - event category.
            dwEventID - event ID.
            bufStrings - list of strings to include in the event.
            bufData - custom event data.
            pUserSid - user SID.

        Returns:

            S_OK - success.
            other - error.
    --*/
    HRESULT ReportEvent(
        WORD wType,
        WORD wCategory,
        DWORD dwEventID,
        const CBuffer<LPCWSTR>& bufStrings,
        const CBuffer<BYTE>& bufData,
        const PSID pUserSid = nullptr) const;

    /*++

        Abstract:

            Wrapper on the Win32 ReportEvent API.

        Arguments:

            wType - event type.
            wCategory - event category.
            dwEventID - event ID.
            bufStrings - list of values to include as strings in the event.
            bufData - custom event data.
            pUserSid - user SID.

        Returns:

            S_OK - success.
            other - error.
    --*/
    HRESULT ReportEvent(
        WORD wType,
        WORD wCategory,
        DWORD dwEventID,
        const CBuffer<CEventArg*>& bufStrings,
        const CBuffer<BYTE>& bufData,
        const PSID pUserSid = nullptr) const;

private:
    LPCWSTR m_pwszProviderName;
    HANDLE m_hEventLog;

    CEventSource(const CEventSource&) = delete;
    CEventSource& operator=(const CEventSource&) = delete;
};

