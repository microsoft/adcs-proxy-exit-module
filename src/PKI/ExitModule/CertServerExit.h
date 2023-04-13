#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    Abstract:

        Wrapper for ICertServerExit.

--*/

#include "CertServerPropType.h"

/*++

    Abstract:

        Wrapper for ICertServerExit.

--*/
class CCertServerExit
{
public:
    /*++
    
        Abstract:
        
            Initializes a new instance of the CCertServerExit class.

    --*/
    CCertServerExit();

    /*++
    
        Abstract:

            Destroys the current instance.

    --*/
    ~CCertServerExit();

    /*++
    
        Abstract:

            Initializes the server interface with an optional context.

        Parameters:

            lContext - the context passed to ICertExit::Notify() or 0.

        Returns:
            
            S_OK for success or an error code.

        Remarks:

            The context is optional during the ICertExit::Initialize() call.
    --*/
    HRESULT Init(
        LONG lContext = 0L);

    /*++
    
        Abstract:

            Releases the underlying interface.

    --*/
    inline void Clear()
    {
        m_ptrInner.Release();
        m_lContext = 0L;
    }

    /*++
    
        Abstract:

            Gets the context id.

        Returns:

            The context id.

        Remarks:

            Use for tracing or anything where a matching number is needed for the context.
    --*/
    inline LONG GetContext() const
    {
        return m_lContext;
    }

    /*++
        
        Abstract:

            Gets the property of a request.

        Parameters:

            pwszName - the property name.
            ePropType - the property type.
            varResult - receives the value of the property.

        Returns:

            S_OK for success. An error code on failure.

    --*/
    HRESULT GetRequestProperty(
        LPCWSTR pwszName,
        CertServerPropType ePropType,
        OUT ATL::CComVariant& varResult) const;

    /*++

        Abstract:

            Gets the property of a certificate (or the server).

        Parameters:

            pwszName - the property name.
            ePropType - the property type.
            varResult - receives the value of the property.

        Returns:

            S_OK for success. An error code on failure.

    --*/
    HRESULT GetCertificateProperty(
        LPCWSTR pwszName,
        CertServerPropType ePropType,
        OUT ATL::CComVariant& varResult) const;

    /*++
    
        Abstract:

            Gets the ModuleRegistryLocation property.

        Parameters:

            strResult - on success, receives the string value.

        Returns:

            S_OK - success.
            Other - error code.
    --*/
    HRESULT GetModuleRegistryLocation(
        OUT CHeapWString& strResult) const;

    /*++

        Abstract:

            Gets the CAType property.

        Parameters:

            eResult - on success, receives the enum value.

        Returns:

            S_OK - success.
            Other - error code.
    --*/
    HRESULT GetCATypeProperty(OUT ENUM_CATYPES& eResult) const
    {
        LONG lResult = 0;
        HRESULT hr = GetCertificateLongProperty(wszPROPCATYPE, lResult);
        eResult = (ENUM_CATYPES)lResult;
        return hr;
    }

    /*++
    
        Abstract:

            Gets the raw certificate property.

        Parameters:

            bufResult - On success, receives the raw bytes of the request.

        Returns:

            S_OK - success.
            Other - error code.
    --*/
    HRESULT GetRawCertificateProperty(OUT CHeapBuffer<BYTE>& bufResult) const;

    /*++

        Abstract:

            Gets the Subject key identifier property.

        Parameters:

            strResult - On success, receives the subject key identifier of the cert.

        Returns:

            S_OK - success.
            Other - error code.
    --*/
    HRESULT GetCertificateSubjectKeyIdentifierProperty(
        OUT CHeapWString& strResult) const
    {
        return GetCertificateStringProperty(
            wszPROPCERTIFICATESUBJECTKEYIDENTIFIER, 
            strResult);
    }

    /*++

        Abstract:

            Gets the serial number property.

        Parameters:

            strResult - On success, receives the subject key identifier of the cert.

        Returns:

            S_OK - success.
            Other - error code.
    --*/
    HRESULT GetCertificateSerialNumberProperty(
        OUT CHeapWString& strResult) const
    {
        return GetCertificateStringProperty(
            wszPROPCERTIFICATESERIALNUMBER,
            strResult);
    }

private:
    ATL::CComPtr<ICertServerExit> m_ptrInner;
    LONG m_lContext;

    static HRESULT CopyString(
        const ATL::CComVariant& var,
        CHeapWString& strResult);

    HRESULT GetCertificateStringProperty(
        LPCWSTR pwszName,
        OUT CHeapWString& strResult) const;

    HRESULT GetCertificateLongProperty(
        LPCWSTR pwszName,
        OUT LONG& lResult) const;

    CCertServerExit(const CCertServerExit&) = delete;
    CCertServerExit& operator=(const CCertServerExit&) = delete;
};

