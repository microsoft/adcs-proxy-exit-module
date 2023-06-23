/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventSource.cpp

    Abstract:

        CEventSource class impl.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        18-Apr-2023 jrowlett Created.

--*/

#include "pch.h"
#include "EventSource.h"

CEventSource::CEventSource(
    LPCWSTR pwszProviderName)
    : m_pwszProviderName(pwszProviderName), m_hEventLog(NULL)
{
}

CEventSource::~CEventSource()
{
    Close();
}

HRESULT CEventSource::Open()
{
    HRESULT hr = S_OK;
    Close();
    m_hEventLog = ::RegisterEventSourceW(
        NULL, // lpUNCServerName
        m_pwszProviderName); // lpSourceName
    if (!m_hEventLog)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"RegisterEventSourceW(%s) failed, hr=%x\n", m_pwszProviderName, hr);
    }

    return hr;
}

void CEventSource::Close()
{
    if (m_hEventLog)
    {
        ::DeregisterEventSource(m_hEventLog);
        m_hEventLog = NULL;
    }
}

HRESULT CEventSource::ReportEvent(
    WORD wType,
    WORD wCategory,
    DWORD dwEventID,
    const CBuffer<LPCWSTR>& bufStrings,
    const CBuffer<BYTE>& bufData,
    const PSID pUserSid /* = nullptr */) const
{
    HRESULT hr = S_OK;
    if (!m_hEventLog)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    }

    if (!::ReportEventW(
        m_hEventLog,
        wType,
        wCategory,
        dwEventID,
        pUserSid,
        (WORD)bufStrings.GetLength(),
        (DWORD)bufData.GetSize(),
        const_cast<LPCWSTR*>(bufStrings.Get()),
        const_cast<LPBYTE>(bufData.Get())))
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"ReportEventW failed, hr=%x\n", hr);
    }

    return hr;
}

HRESULT CEventSource::ReportEvent(
    WORD wType,
    WORD wCategory,
    DWORD dwEventID,
    const CBuffer<CEventArg*>& bufStrings,
    const CBuffer<BYTE>& bufData,
    const PSID pUserSid /* = nullptr */) const
{
    CHeapBuffer<LPCWSTR> bufFormattedStrings;
    HRESULT hr = S_OK;
    if (bufStrings.GetLength() > 0)
    {
        if (!bufFormattedStrings.Alloc(bufStrings.GetLength()))
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }

        for (size_t i = 0; i < bufStrings.GetLength(); i++)
        {
            CEventArg* arg = bufStrings.Get()[i];
            LPCWSTR& pwszResult = bufFormattedStrings.Get()[i];
            hr = arg->Format(OUT pwszResult);
            if (FAILED(hr))
            {
                return hr;
            }
        }
    }

    return ReportEvent(
        wType,
        wCategory,
        dwEventID,
        bufFormattedStrings,
        bufData, 
        pUserSid);
}