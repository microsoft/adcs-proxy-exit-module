/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        CertServerExit.cpp

    Abstract:

        Wrapper for ICertServerExit.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        10-Apr-2023 jrowlett Created.

--*/

#include "pch.h"
#include "CertServerExit.h"

CCertServerExit::CCertServerExit()
    : m_ptrInner(), m_lContext(0L)
{
}

CCertServerExit::~CCertServerExit()
{
}

HRESULT CCertServerExit::Init(
    LONG lContext /* = 0L */)
{
    HRESULT hr = S_OK;

    do
    {
        Clear();

        hr = m_ptrInner.CoCreateInstance(
            CLSID_CCertServerExit,
            nullptr, // pUnkOuter
            CLSCTX_INPROC_SERVER);
        if (FAILED(hr))
        {
            ATLTRACE(L"CCI for CLSID_CCertServerExit failed, hr=%x\n", hr);
            break;
        }

        if (lContext != 0)
        {
            hr = m_ptrInner->SetContext(lContext);
            if (FAILED(hr))
            {
                ATLTRACE(L"ICertServerExit::SetContext(%x) failed, hr=%x\n", lContext, hr);
                break;
            }
        }

        m_lContext = lContext;
    } while (false);

    if (FAILED(hr))
    {
        Clear();
    }

    return hr;
}

HRESULT CCertServerExit::GetRequestProperty(
    LPCWSTR pwszName,
    CertServerPropType ePropType,
    OUT ATL::CComVariant& varResult) const
{
    HRESULT hr = S_OK;
    ATL::CComBSTR bstrName;

    do
    {
        if (!m_ptrInner)
        {
            hr = HRESULT_FROM_WIN32(ERROR_INTERNAL_ERROR);
            ATLTRACE(L"m_ptrInner is null.\n");
            break;
        }

        hr = bstrName.Append(pwszName);
        if (FAILED(hr))
        {
            ATLTRACE(L"bstrName.Append failed, hr=%x\n", hr);
            break;
        }

        varResult.Clear();
        hr = m_ptrInner->GetRequestProperty(bstrName, ePropType, &varResult);
        if (FAILED(hr))
        {
            ATLTRACE(
                L"ICertServerExit::GetRequestProperty(%s, %x) failed, hr=%x\n",
                pwszName,
                ePropType,
                hr);
            break;
        }

    } while (false);

    return hr;
}

HRESULT CCertServerExit::GetCertificateProperty(
    LPCWSTR pwszName,
    CertServerPropType ePropType,
    OUT ATL::CComVariant& varResult) const
{
    HRESULT hr = S_OK;
    ATL::CComBSTR bstrName;

    do
    {
        if (!m_ptrInner)
        {
            hr = HRESULT_FROM_WIN32(ERROR_INTERNAL_ERROR);
            ATLTRACE(L"m_ptrInner is null.\n");
            break;
        }

        hr = bstrName.Append(pwszName);
        if (FAILED(hr))
        {
            ATLTRACE(L"bstrName.Append failed, hr=%x\n", hr);
            break;
        }

        varResult.Clear();
        hr = m_ptrInner->GetCertificateProperty(bstrName, ePropType, &varResult);
        if (FAILED(hr))
        {
            ATLTRACE(
                L"ICertServerExit::GetCertificateProperty(%s, %x) failed, hr=%x\n",
                pwszName,
                ePropType,
                hr);
            break;
        }

    } while (false);

    return hr;
}

HRESULT CCertServerExit::GetModuleRegistryLocation(
    OUT CHeapWString& strResult) const
{
    return GetCertificateStringProperty(
        wszPROPMODULEREGLOC,
        OUT strResult);
}

HRESULT CCertServerExit::GetRawCertificateProperty(
    OUT CHeapBuffer<BYTE>& bufResult) const
{
    HRESULT hr = S_OK;
    ATL::CComVariant var;

    do
    {
        hr = GetCertificateProperty(
            wszPROPRAWCERTIFICATE,
            CertServerPropType::PropTypeBinary,
            OUT var);
        if (FAILED(hr))
        {
            // already traced.
            break;
        }

        if (var.vt != VT_BSTR)
        {
            ATLTRACE(L"Expected VT_BSTR, actual=%d\n", var.vt);
            hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
            break;
        }

        size_t cb = ::SysStringByteLen(var.bstrVal);
        ATLTRACE(L"Raw cert size in bytes=%x\n", cb);

        if (!bufResult.Alloc(cb))
        {
            hr = E_OUTOFMEMORY;
            ATLTRACE(L"Failed to alloc heap for raw cert.\n");
            break;
        }

        CopyMemory(bufResult.Get(), var.bstrVal, cb);
    } while (false);

    return hr;
}

HRESULT CCertServerExit::CopyString(
    const ATL::CComVariant& var,
    CHeapWString& strResult)
{
    return strResult.Copy(var.bstrVal);
}

HRESULT CCertServerExit::GetCertificateStringProperty(
    LPCWSTR pwszName,
    OUT CHeapWString& strResult) const
{
    ATL::CComVariant var;
    HRESULT hr = GetCertificateProperty(
        pwszName,
        CertServerPropType::PropTypeString,
        OUT var);
    if (SUCCEEDED(hr))
    {
        hr = CopyString(var, strResult);
    }

    return hr;
}

HRESULT CCertServerExit::GetCertificateLongProperty(
    LPCWSTR pwszName,
    OUT LONG& lResult) const
{
    ATL::CComVariant var;
    HRESULT hr = GetCertificateProperty(
        pwszName,
        CertServerPropType::PropTypeLong,
        OUT var);
    if (SUCCEEDED(hr))
    {
        lResult = var.lVal;
    }

    return hr;
}