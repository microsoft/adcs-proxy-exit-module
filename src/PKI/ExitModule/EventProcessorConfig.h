#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventProcessorConfig.h

    Abstract:

        CEventProcessorConfig class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        14-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:

        Registry Configuration used by the event processor.
--*/
class CEventProcessorConfig
{
public:
    CEventProcessorConfig();
    ~CEventProcessorConfig();

    /*++
    
        Abstract:

            Loads config values from the registry.

        Returns:

            S_OK - success.
            other - error.
    --*/
    HRESULT Init();

    inline LPCWSTR GetExePath() const
    {
        return m_strExePath.Get();
    }

    inline const CBuffer<LPCWSTR>& GetArguments() const
    {
        return m_bufArguments;
    }

    inline bool GetEscapeForPS() const
    {
        return m_fEscapeForPS;
    }

private:
    CHeapWString m_strExePath;
    CHeapBuffer<WCHAR> m_bufArgData;
    CHeapBuffer<LPCWSTR> m_bufArguments;
    bool m_fEscapeForPS;

    CEventProcessorConfig(const CEventProcessorConfig&) = delete;
    CEventProcessorConfig& operator=(const CEventProcessorConfig&) = delete;
};

