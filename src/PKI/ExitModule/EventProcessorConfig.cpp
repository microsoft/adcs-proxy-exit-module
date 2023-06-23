/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventProcessorConfig.cpp

    Abstract:

        CEventProcessorConfig class impl.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        14-Apr-2023 jrowlett Created.

--*/
#include "pch.h"
#include "EventProcessorConfig.h"

LPCWSTR g_pwszRegSubkey = L"Software\\Microsoft\\PMI\\PMIExitModule";
LPCWSTR g_pwszExePathValueName = L"ExePath";
LPCWSTR g_pwszArgumentsValueName = L"Arguments";
LPCWSTR g_pwszEscapeForPSValueName = L"EscapeForPS";

constexpr const size_t g_cbRegValueBuffer = 1024;

CEventProcessorConfig::CEventProcessorConfig()
    : m_fEscapeForPS(false)
{
}

CEventProcessorConfig::~CEventProcessorConfig()
{
}

HRESULT CEventProcessorConfig::Init()
{
    HRESULT hr = S_OK;
    LSTATUS lr = ERROR_SUCCESS;
    ATL::CRegKey keyModule;
    DWORD dwType = 0;
    DWORD cbBuf = 0;

    do
    {
        lr = keyModule.Open(
            HKEY_LOCAL_MACHINE,
            g_pwszRegSubkey,
            KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE);
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
        lr = keyModule.QueryValue(
            g_pwszExePathValueName,
            &dwType,
            (BYTE*)m_strExePath.Get(),
            &cbBuf);
        if (lr != ERROR_SUCCESS)
        {
            hr = HRESULT_FROM_WIN32(lr);
            ATLTRACE(L"Failed to query reg value %s, hr=%x\n", g_pwszExePathValueName, hr);
            break;
        }

        if (dwType != REG_SZ && dwType != REG_EXPAND_SZ)
        {
            ATLTRACE(L"Expected REG_SZ, actual %d\n", dwType);
            hr = E_UNEXPECTED;
            break;
        }

        DWORD dwEscapeForPS = 0;
        lr = keyModule.QueryDWORDValue(
            g_pwszEscapeForPSValueName,
            OUT dwEscapeForPS);
        if (lr != ERROR_SUCCESS)
        {
            // optional. ignore failure.
            ATLTRACE(
                L"Failed to query optional reg value %s, hr=%x\n",
                g_pwszEscapeForPSValueName,
                HRESULT_FROM_WIN32(lr));
        }
        else
        {
            m_fEscapeForPS = (dwEscapeForPS != 0);
        }

        if (!m_bufArgData.Alloc(g_cbRegValueBuffer))
        {
            ATLTRACE(L"Failed to alloc wchars for args.\n");
            hr = E_OUTOFMEMORY;
            break;
        }

        ULONG cch = (ULONG)m_bufArgData.GetLength();
        lr = keyModule.QueryMultiStringValue(
            g_pwszArgumentsValueName,
            m_bufArgData.Get(),
            &cch);
        if (lr != ERROR_SUCCESS)
        {
            // optional. ignore failure.
            ATLTRACE(
                L"Failed to query optional reg value %s, hr=%x\n",
                g_pwszArgumentsValueName,
                HRESULT_FROM_WIN32(lr));
        }
        else
        {
            size_t cArgs = 0;
            size_t cchMax = cch;
            LPCWSTR pwsz = m_bufArgData.Get();
            while (pwsz && *pwsz)
            {
                size_t cchArg = 0;
                hr = ::StringCchLengthW(pwsz, cchMax, &cchArg);
                if (FAILED(hr))
                {
                    break;
                }

                cArgs++;
                cchMax -= cchArg + 1;
                pwsz += cchArg + 1;
            }

            if (!m_bufArguments.Alloc(cArgs))
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            pwsz = m_bufArgData.Get();
            cchMax = cch;
            for (UINT nIndex = 0; pwsz && *pwsz; nIndex++)
            {
                size_t cchArg = 0;
                hr = ::StringCchLengthW(pwsz, cchMax, &cchArg);
                if (FAILED(hr))
                {
                    break;
                }

                m_bufArguments.Get()[nIndex] = pwsz;

                cchMax -= (ULONG)cchArg + 1;
                pwsz += cchArg + 1;
            }
        }
        
    } while (false);

    return hr;
}
