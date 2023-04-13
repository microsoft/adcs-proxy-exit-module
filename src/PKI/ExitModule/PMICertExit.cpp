// PMICertExit.cpp : Implementation of CPMICertExit

#include "pch.h"
#include "EventProcessor.h"
#include "PMICertExit.h"
#include "PMIExitModule.h"
#include "CertServerExit.h"

#define ALL_EXITEVENTS \
    (EXITEVENT_CERTDENIED | \
    EXITEVENT_CERTISSUED | \
    EXITEVENT_CERTPENDING | \
    EXITEVENT_CERTRETRIEVEPENDING | \
    EXITEVENT_CERTREVOKED | \
    EXITEVENT_CRLISSUED | \
    EXITEVENT_SHUTDOWN | \
    EXITEVENT_CERTIMPORTED)

const IID* CPMICertExit::s_rgErrorInfoInterfaces[] =
{
    &IID_ICertExit,
};

// CPMICertExit

 STDMETHODIMP CPMICertExit::Initialize(
    /* [in] */ __RPC__in const BSTR strConfig,
    /* [retval][out] */ __RPC__out LONG* pEventMask)
{
    HRESULT hr = S_OK;
    CCertServerExit objServer;
    ATLTRACE(L"Enter CPMICertExit::Initialize. strConfig=%p, pEventMask=%p\n", strConfig, pEventMask);

    if (!pEventMask)
    {
        return E_POINTER;
    }

    do
    {
        *pEventMask = ALL_EXITEVENTS;

        if (!strConfig)
        {
            hr = E_INVALIDARG;
            ATLTRACE(L"strConfig is null.\n");
            break;
        }

        hr = m_strConfig.Copy(strConfig);
        if (FAILED(hr))
        {
            ATLTRACE(L"Failed to copy strConfig, hr=%x\n", hr);
            break;
        }

        hr = objServer.Init();
        if (FAILED(hr))
        {
            ATLTRACE(L"Failed to init objServer, hr=%x\n", hr);
            break;
        }

        hr = objServer.GetModuleRegistryLocation(OUT m_strRegStorageLoc);
        if (FAILED(hr))
        {
            // TODO: This is failing. Understand why.
            ATLTRACE(L"Failed to get ModuleRegistryLocation, hr=%x\n", hr);
            hr = S_OK;
        }

        hr = objServer.GetCATypeProperty(OUT m_eCAType);
        if (FAILED(hr))
        {
            ATLTRACE(L"Failed to get CAType, hr=%x\n", hr);
            hr = S_OK;
        }

        ATLTRACE(L"m_strConfig=%s\n", m_strConfig.Get());
        ATLTRACE(L"m_strRegStorageLoc=%s\n", m_strRegStorageLoc.Get());
        ATLTRACE(L"m_eCAType=%d\n", m_eCAType);

    } while (false);

    ATLTRACE(L"Leave CPMICertExit::Initialize. hr=%x\n", hr);
    return hr;
}
 
STDMETHODIMP CPMICertExit::Notify(
    /* [in] */ LONG ExitEvent,
    /* [in] */ LONG Context)
{
    HRESULT hr = S_OK;
    ATLTRACE(L"Enter CPMICertExit::Notify. ExitEvent=%x, Context=%x\n", ExitEvent, Context);

    switch (ExitEvent)
    {
    case EXITEVENT_CERTISSUED:
        hr = NotifyCertIssued(Context);
        break;

    case EXITEVENT_CERTPENDING:
        hr = NotifyCertPending(Context);
        break;

    case EXITEVENT_CERTDENIED:
        hr = NotifyCertDenied(Context);
        break;

    case EXITEVENT_CERTREVOKED:
        hr = NotifyCertRevoked(Context);
        break;

    case EXITEVENT_CERTRETRIEVEPENDING:
        hr = NotifyCertRetrievePending(Context);
        break;

    case EXITEVENT_CRLISSUED:
        hr = NotifyCRLIssued(Context);
        break;

    case EXITEVENT_SHUTDOWN:
        hr = NotifyShutdown(Context);
        break;

    case EXITEVENT_CERTIMPORTED:
        hr = NotifyCertImported(Context);
        break;

    default:
        // TODO: Trace.
        break;
    }

    ATLTRACE(L"Leave CPMICertExit::Notify. hr=%x\n", hr);
    return hr;
}

STDMETHODIMP CPMICertExit::GetDescription(
    /* [retval][out] */ __RPC__deref_out_opt BSTR* pstrDescription)
{
    if (!pstrDescription)
    {
        return E_POINTER;
    }

    *pstrDescription = ::SysAllocString(L"Hello there!");
    return S_OK;
}

STDMETHODIMP CPMICertExit::GetManageModule(
    /* [retval][out] */ __RPC__deref_out_opt ICertManageModule** ppManageModule)
{
    if (!ppManageModule)
    {
        return E_POINTER;
    }

    *ppManageModule = nullptr;
    CComObject<CPMIExitModule>* pModule = nullptr;
    HRESULT hr = S_OK;

    do
    {
        hr = CComObject<CPMIExitModule>::CreateInstance(&pModule);
        if (FAILED(hr))
        {
            break;
        }

        hr = pModule->QueryInterface<ICertManageModule>(ppManageModule);
        if (FAILED(hr))
        {
            delete pModule;
        }
    } while (false);

    return hr;
}

HRESULT CPMICertExit::NotifyCertIssued(
    IN CCertServerExit& objServer)
{
    HRESULT hr = S_OK;
    CHeapBuffer<BYTE> buf;
    CHeapWString strSubjectKeyIdentifier;
    CHeapWString strSerialNumber;
    CEventProcessor objEventProcessor;

    do
    {
        hr = objServer.GetCertificateSubjectKeyIdentifierProperty(OUT strSubjectKeyIdentifier);
        if (FAILED(hr))
        {
            ATLTRACE(L"CCertServerExit::GetCertificateSubjectKeyIdentifierProperty failed, hr=%x\n");
            break;
        }

        hr = objServer.GetCertificateSerialNumberProperty(OUT strSerialNumber);
        if (FAILED(hr))
        {
            ATLTRACE(L"CCertServerExit::GetCertificateSerialNumberProperty failed, hr=%x\n", hr);
            break;
        }

        hr = objServer.GetRawCertificateProperty(OUT buf);
        if (FAILED(hr))
        {
            ATLTRACE(L"CCertServerExit::GetRawCertificateProperty failed, hr=%x\n", hr);
            break;
        }

        ATLTRACE(
            L"Cert created. Subject Key Identifier=[%s], SerialNumber=[%s]\n",
            strSubjectKeyIdentifier.Get(),
            strSerialNumber.Get());
        ATLTRACE(L"Raw cert size in bytes=%x\n", buf.GetSize());

        hr = objEventProcessor.Init();
        if (FAILED(hr))
        {
            ATLTRACE(L"CEventProcessor::Init failed, hr=%x\n", hr);
            break;
        }

        hr = objEventProcessor.NotifyCertIssued(
            strSubjectKeyIdentifier.Get(),
            strSerialNumber.Get(),
            buf);
        if (FAILED(hr))
        {
            ATLTRACE(L"CEventProcessor::NotifyCertIssued failed, hr=%x\n", hr);
            break;
        }
    } while (false);

    return hr;
}

HRESULT CPMICertExit::NotifyCRLIssued(
    IN CCertServerExit& /* objServer */)
{
    return S_OK;
}

STDMETHODIMP CPMICertExit::InterfaceSupportsErrorInfo(
    /* [in] */ __RPC__in REFIID riid)
{
    for (
        const IID** p = s_rgErrorInfoInterfaces;
        p != s_rgErrorInfoInterfaces + sizeof(s_rgErrorInfoInterfaces) / sizeof(s_rgErrorInfoInterfaces[0]);
        p++)
    {
        if (IsEqualGUID(**p, riid))
        {
            return S_OK;
        }
    }

    return S_FALSE;
}

HRESULT CPMICertExit::NotifyCertIssued(LONG lContext)
{
    CCertServerExit obj;
    HRESULT hr = obj.Init(lContext);
    if (SUCCEEDED(hr))
    {
        hr = NotifyCertIssued(obj);
    }

    return hr;
}

HRESULT CPMICertExit::NotifyCertPending(LONG /* lContext */)
{
    return S_OK;
}

HRESULT CPMICertExit::NotifyCertDenied(LONG /* lContext */)
{
    return S_OK;
}

HRESULT CPMICertExit::NotifyCertRevoked(LONG /* lContext */)
{
    return S_OK;
}

HRESULT CPMICertExit::NotifyCertRetrievePending(LONG /* lContext */)
{
    return S_OK;
}

HRESULT CPMICertExit::NotifyCRLIssued(LONG lContext)
{
    CCertServerExit obj;
    HRESULT hr = obj.Init(lContext);
    if (SUCCEEDED(hr))
    {
        hr = NotifyCertIssued(obj);
    }

    return hr;
}

HRESULT CPMICertExit::NotifyShutdown(LONG /* lContext */)
{
    return S_OK;
}

HRESULT CPMICertExit::NotifyCertImported(LONG /* lContext */)
{
    return S_OK;
}