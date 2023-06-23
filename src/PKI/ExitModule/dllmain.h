// dllmain.h : Declaration of module class.

class CExitModuleModule : public ATL::CAtlDllModuleT< CExitModuleModule >
{
public :
	DECLARE_LIBID(LIBID_ExitModuleLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EXITMODULE, "{03c5878a-932c-4063-b21a-5f89f579194f}")
};

extern class CExitModuleModule _Module;
