#pragma once
/*++

	Copyright (C) Microsoft Corp. All rights reserved.

	Abstract:

		Definition of properties used by CPMIExitModule.

--*/
#include "ManageProperty.h"

/*++

    Abstract:

        A string property that gets loaded from the string table.
--*/
class CResourceStringManageProperty :
    public CManageProperty
{
public:
	/*++
		Abstract:
			
			Initializes a new instance of the CManageProperty class.

		Parameters:
			pwszName - static string for the property name.
			nResID - resource id of the string.

	--*/
	CResourceStringManageProperty(LPCWSTR pwszName, int nResID)
		: CManageProperty(pwszName), m_nResID(nResID)
	{
	}

	/*++
		Gets the value of the property.

		Parameters:
			rvarResult - reference to the variant to receive the value.

		Returns:
			S_OK - success
			Other - error code.
	--*/
	virtual HRESULT GetValue(VARIANT& rvarResult) const;

private:
	int m_nResID;
};

