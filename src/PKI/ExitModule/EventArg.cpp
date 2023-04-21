/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventArg.cpp

    Abstract:

        CEventArg and dervived class impls.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        20-Apr-2023 jrowlett Created.

--*/

#include "pch.h"
#include "EventArg.h"

const LPCWSTR CNumericEventArg<DWORD>::s_pwszFormatString = L"%u";
const LPCWSTR CNumericEventArg<HRESULT>::s_pwszFormatString = L"%x";
constexpr const size_t g_cchMessage = 4096;

HRESULT CStringEventArg::Format(OUT LPCWSTR& rpwszResult)
{
    rpwszResult = m_pwszValue;
    return S_OK;
}

HRESULT CErrorMessageEventArg::Format(OUT LPCWSTR& rpwszResult)
{
    if (!m_bufMessage.Alloc(g_cchMessage))
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = S_OK;
    DWORD cch = ::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, // lpSource
        m_hr,
        LANG_SYSTEM_DEFAULT,
        m_bufMessage.Get(),
        (DWORD)m_bufMessage.GetLength(),
        nullptr);
    if (cch == 0)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"FormatMessage failed for error=%x, hr=%x\n", m_hr, hr);
        m_bufMessage.Clear();
    }

    rpwszResult = m_bufMessage.Get();
    return hr;
}