/*++

	Copyright (C) Microsoft Corp. All rights reserved.

	Abstract:

		Definition of properties used by CPMIExitModule.

--*/

#include "pch.h"
#include "ResourceStringManageProperty.h"
#include "PMIExitModule.h"

/*++
	Gets the value of the property.

	Parameters:
		rvarResult - reference to the variant to receive the value.

	Returns:
		S_OK - success
		Other - error code.
--*/
HRESULT CResourceStringManageProperty::GetValue(VARIANT& rvarResult) const
{
	CStaticBuffer<WCHAR, 1024> wszBuffer;
	int cch = ::LoadStringW(
		_AtlBaseModule.GetModuleInstance(),
		m_nResID,
		wszBuffer.Get(),
		(int)wszBuffer.GetLength());
	if (!cch)
	{
		int nStatus = ::GetLastError();
		return HRESULT_FROM_WIN32(nStatus);
	}

	BSTR bstrResult = ::SysAllocString(wszBuffer.Get());
	if (!bstrResult)
	{
		return E_OUTOFMEMORY;
	}

	rvarResult.bstrVal = bstrResult;
	rvarResult.vt = VT_BSTR;
	return S_OK;
}