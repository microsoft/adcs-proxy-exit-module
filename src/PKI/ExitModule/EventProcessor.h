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
    CHeapWString m_strExePath;

    static HRESULT GetTempFilePath(
        OUT CHeapWString& strPath);
    HRESULT RunProcess(
        const CBuffer<LPCWSTR>& bufArgs,
        OUT DWORD& dwExitCode) const;


    CEventProcessor(const CEventProcessor&) = delete;
    CEventProcessor& operator=(const CEventProcessor&) = delete;
};

