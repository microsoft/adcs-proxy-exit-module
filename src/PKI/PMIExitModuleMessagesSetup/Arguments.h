#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        Arguments.h

    Abstract:

        CArguments class decl.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        24-Apr-2023 jrowlett Created.

--*/
#include <string>

/*++

    Abstract:

        The operation to perform.

--*/
enum Operation
{
    INSTALL,
    UINSTALL
};

/*++

    Abstract:

        Parsed program arguments.

--*/
class CArguments
{
public:
    CArguments();
    ~CArguments();

    /*++
    
        Abstract:

            Tries to parse the arguments.

        Arguments:

            argc - count of program arguments.
            argv - array of program arguments.

        Returns:

            true - the arguments were parsed.
            false - the argument were invalid.

    --*/
    bool TryParse(int argc, const wchar_t* argv[]);

    inline Operation GetOperation() const
    {
        return m_eOperation;
    }

    inline const std::wstring& GetPath() const
    {
        return m_strPath;
    }

private:
    Operation m_eOperation;
    std::wstring m_strPath;
};

