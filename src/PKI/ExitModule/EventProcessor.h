#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventProcessor.h

    Abstract:

        CEventProcessor class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        11-Apr-2023 jrowlett Created.

--*/

#include "EventProcessorConfig.h"

/*++

    Abstract:

        Processes structured events from the exit module by calling an external process.

--*/
class CEventProcessor
{
public:
    CEventProcessor();
    ~CEventProcessor();

    HRESULT Init();

    HRESULT NotifyCertIssued(
        LPCWSTR pwszSubjectKeyIdentifier,
        LPCWSTR pwszSerialNumber,
        const CBuffer<BYTE>& bufRawCert) const;

private:
    CEventProcessorConfig m_objConfig;

    static HRESULT GetTempFilePath(
        OUT CHeapWString& strPath);
    static HRESULT EscapeArgumentForPS(
        LPCWSTR pwsz,
        OUT CHeapWString& strResult);
    HRESULT RunProcess(
        const CBuffer<LPCWSTR>& bufArgs,
        OUT DWORD& dwExitCode) const;

    CEventProcessor(const CEventProcessor&) = delete;
    CEventProcessor& operator=(const CEventProcessor&) = delete;
};

