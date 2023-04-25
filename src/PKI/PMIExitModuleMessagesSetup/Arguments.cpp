/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        Arguments.cpp

    Abstract:

        CArguments class impl.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        24-Apr-2023 jrowlett Created.

--*/
#include "Arguments.h"

CArguments::CArguments()
    : m_eOperation(Operation::INSTALL)
{
}

CArguments::~CArguments()
{
}

bool CArguments::TryParse(
    int argc,
    const wchar_t* argv[])
{
    int i = 1;
    const wchar_t* pArg = nullptr;
    if (i < argc)
    {
        pArg = argv[i];
        if (pArg[0] && pArg[0] == L'/')
        {
            if (wcscmp(pArg, L"/i") == 0)
            {
                m_eOperation = Operation::INSTALL;
            }
            else if (wcscmp(pArg, L"/u") == 0)
            {
                m_eOperation = Operation::UINSTALL;
            }
            else
            {
                return false;
            }
        }

        i++;
    }

    if (i < argc)
    {
        m_strPath = argv[i];
        if (m_eOperation != Operation::INSTALL)
        {
            return false;
        }

        i++;
    }

    if (i != argc)
    {
        // Extra args.
        return false;
    }

    return true;
}