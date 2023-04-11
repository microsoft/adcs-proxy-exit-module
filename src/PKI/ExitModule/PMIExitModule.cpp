// PMIExitModule.cpp : Implementation of CPMIExitModule

#include "pch.h"
#include "PMIExitModule.h"
#include "ResourceStringManageProperty.h"

const CResourceStringManageProperty propName(
    wszCMM_PROP_NAME,
    IDS_PMIEXITMODULE_NAME);
const CResourceStringManageProperty propDesc(
    wszCMM_PROP_DESCRIPTION,
    IDS_PMIEXITMODULE_DESC);
const CResourceStringManageProperty propCopyright(
    wszCMM_PROP_COPYRIGHT,
    IDS_PMIEXITMODULE_COPYRIGHT);
const CResourceStringManageProperty propFileVer(
    wszCMM_PROP_FILEVER,
    IDS_PMIEXITMODULE_FILEVER);
const CResourceStringManageProperty propProductVer(
    wszCMM_PROP_PRODUCTVER,
    IDS_PMIEXITMODULE_PRODUCTVER);
const CManageProperty* CPMIExitModule::s_rgProperties[] =
{
    &propName,
    &propDesc,
    &propCopyright,
    &propFileVer,
    &propProductVer,
};

// CPMIExitModule

STDMETHODIMP CPMIExitModule::GetProperty(
    /* [in] */ const BSTR /* strConfig */,
    /* [in] */ BSTR /* strStorageLocation */,
    /* [in] */ BSTR strPropertyName,
    /* [in] */ LONG /* dwFlags */,
    /* [retval][out] */ VARIANT __RPC_FAR* pvarProperty)
{
    if (!pvarProperty)
    {
        return E_POINTER;
    }

    ::VariantInit(pvarProperty);

    HRESULT hr = S_FALSE;
    do
    {
        if (!strPropertyName || ::SysStringByteLen(strPropertyName) == 0)
        {
            break;
        }

        for (const CManageProperty** p = s_rgProperties;
            p != s_rgProperties + (sizeof(s_rgProperties) / sizeof(s_rgProperties[0]));
            p++)
        {
            const CManageProperty* pProp = *p;
            if (_wcsicmp(strPropertyName, pProp->GetName()) == 0)
            {
                hr = pProp->GetValue(*pvarProperty);
                break;
            }
        }
    } while (false);

    return hr;
}

STDMETHODIMP CPMIExitModule::SetProperty(
    /* [in] */ const BSTR /* strConfig */,
    /* [in] */ BSTR /* strStorageLocation */,
    /* [in] */ BSTR /* strPropertyName */,
    /* [in] */ LONG /* dwFlags */,
    /* [in] */ VARIANT const __RPC_FAR* /* pvarProperty */)
{
    // no settable properties supported
    return S_FALSE;
}

STDMETHODIMP CPMIExitModule::Configure(
    /* [in] */ const BSTR /* strConfig */,
    /* [in] */ BSTR /* strStorageLocation */,
    /* [in] */ LONG /* dwFlags */)
{
    // TODO: Show a message box.
    return S_OK;
}