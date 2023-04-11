// PMIExitModule.h : Declaration of the CPMIExitModule

#pragma once
#include "resource.h"       // main symbols

#include "ManageProperty.h"

#include "ExitModule_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// Get the definition of _Module used by ATL macros.
#include "dllmain.h"

// CPMIExitModule

class ATL_NO_VTABLE CPMIExitModule :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPMIExitModule, &CLSID_PMIExitModule>,
	public IDispatchImpl<ICertManageModule, &IID_ICertManageModule, &LIBID_ExitModuleLib>
{
public:
	CPMIExitModule()
	{
	}

DECLARE_REGISTRY_RESOURCEID(106)

BEGIN_COM_MAP(CPMIExitModule)
	COM_INTERFACE_ENTRY(ICertManageModule)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	virtual HRESULT STDMETHODCALLTYPE GetProperty(
        /* [in] */ const BSTR strConfig,
        /* [in] */ BSTR strStorageLocation,
        /* [in] */ BSTR strPropertyName,
        /* [in] */ LONG dwFlags,
        /* [retval][out] */ VARIANT __RPC_FAR* pvarProperty);

	virtual HRESULT STDMETHODCALLTYPE SetProperty(
        /* [in] */ const BSTR strConfig,
        /* [in] */ BSTR strStorageLocation,
        /* [in] */ BSTR strPropertyName,
        /* [in] */ LONG dwFlags,
        /* [in] */ VARIANT const __RPC_FAR* pvarProperty);

	virtual HRESULT STDMETHODCALLTYPE Configure(
        /* [in] */ const BSTR strConfig,
        /* [in] */ BSTR strStorageLocation,
        /* [in] */ LONG dwFlags);

private:
	static const CManageProperty* s_rgProperties[];
};

OBJECT_ENTRY_AUTO(__uuidof(PMIExitModule), CPMIExitModule)
