#pragma once
/*++
	
	Copyright (C) Microsoft Corp. All rights reserved.

	Abstract:

		Definition of properties used by CPMIExitModule.

--*/
#include "framework.h"

/*++

	Abstract:

		Base class for properties used by CPMIExitModule.
--*/
class CManageProperty
{
public:

	/*++
		Gets the name of the property.

		Returns:
			A constant pointer to the name string.
	--*/
	inline LPCWSTR GetName() const
	{
		return m_pwszName;
	}

	/*++
		Gets the value of the property.

		Parameters:
			rvarResult - reference to the variant to receive the value.

		Returns:
			S_OK - success
			Other - error code.
	--*/
	virtual HRESULT GetValue(VARIANT& rvarResult) const = 0;

protected:
	/*++
		Abstract:

			Initializes a new instance of the CManageProperty class.

		Parameters:
			pwszName - static string for the property name.

	--*/
	CManageProperty(LPCWSTR pwszName)
		: m_pwszName(pwszName)
	{
	}

private:
	LPCWSTR m_pwszName;
};
