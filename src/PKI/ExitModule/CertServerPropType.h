#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        CertServerPropType.h

    Abstract:

        CertServerPropType enum

    Authors:

        Jon Rowlett (jrowlett)

    History:
        10-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:

        Enum to wrap the DWORD Certsrv.h has for property types.

--*/
typedef enum _CertServerPropType : DWORD
{
    // signed long.
    PropTypeLong = PROPTYPE_LONG,

    // date+time.
    PropTypeDate = PROPTYPE_DATE,

    // binary data.
    PropTypeBinary = PROPTYPE_BINARY,

    // string.
    PropTypeString = PROPTYPE_STRING,
} CertServerPropType;