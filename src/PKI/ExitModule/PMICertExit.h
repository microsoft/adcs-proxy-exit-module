// PMICertExit.h : Declaration of the CPMICertExit

#pragma once
#include "resource.h"       // main symbols



#include "ExitModule_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CPMICertExit

class ATL_NO_VTABLE CPMICertExit :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPMICertExit, &CLSID_PMICertExit>,
	public IDispatchImpl<ICertExit2, &IID_ICertExit2, &LIBID_ExitModuleLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CPMICertExit()
	{
	}

DECLARE_REGISTRY_RESOURCEID(107)


BEGIN_COM_MAP(CPMICertExit)
	COM_INTERFACE_ENTRY(ICertExit)
	COM_INTERFACE_ENTRY(ICertExit2)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
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

	/*
		ICertExit
	*/

	virtual HRESULT STDMETHODCALLTYPE Initialize(
		/* [in] */ __RPC__in const BSTR strConfig,
		/* [retval][out] */ __RPC__out LONG* pEventMask);

	virtual HRESULT STDMETHODCALLTYPE Notify(
		/* [in] */ LONG ExitEvent,
		/* [in] */ LONG Context);

	virtual HRESULT STDMETHODCALLTYPE GetDescription(
		/* [retval][out] */ __RPC__deref_out_opt BSTR* pstrDescription);

	/*
		ICertExit2
	*/

	virtual HRESULT STDMETHODCALLTYPE GetManageModule(
		/* [retval][out] */ __RPC__deref_out_opt ICertManageModule** ppManageModule);

	/*
		ISupportErrorInfo
	*/

	virtual HRESULT STDMETHODCALLTYPE InterfaceSupportsErrorInfo(
		/* [in] */ __RPC__in REFIID riid);

protected:
	HRESULT NotifyCertIssued(IN CCertServerExit& objServer);
	HRESULT NotifyCRLIssued(IN CCertServerExit& objServer);

private:
	/*
		Array of interfaces that support error info.
	*/
	static const IID* s_rgErrorInfoInterfaces[];

	CHeapWString m_strConfig;
	CHeapWString m_strRegStorageLoc;
	ENUM_CATYPES m_eCAType;
	CPMIExitModuleEventSource m_objEventSource;

	HRESULT NotifyCertIssued(LONG lContext);
	HRESULT NotifyCertPending(LONG lContext);
	HRESULT NotifyCertDenied(LONG lContext);
	HRESULT NotifyCertRevoked(LONG lContext);
	HRESULT NotifyCertRetrievePending(LONG lContext);
	HRESULT NotifyCRLIssued(LONG lContext);
	HRESULT NotifyShutdown(LONG lContext);
	HRESULT NotifyCertImported(LONG lContext);
};

OBJECT_ENTRY_AUTO(__uuidof(PMICertExit), CPMICertExit)
