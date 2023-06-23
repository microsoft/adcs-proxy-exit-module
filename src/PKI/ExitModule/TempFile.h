#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        TempFile.h

    Abstract:

        CTempFile class declaration.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        11-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:

        Encapsulates access to a temp file that gets deleted on destruction.
--*/
class CTempFile
{
public:
    CTempFile()
        : m_hFile(INVALID_HANDLE_VALUE), m_fPreserve(false)
    {
    }

    ~CTempFile();

    /*++
    
        Abstract:

            Creates and opens the temp file.

        Parameters:

            pwszPath - path to the file.

        Returns:

            S_OK - success.
            other - error code.
    --*/
    HRESULT Create(LPCWSTR pwszPath);

    /*++
    
        Abstract:

            Closes the file, but does not delete it.

    --*/
    void Close();

    /*++
    
        Abstract:

            Prevents the file from getting deleted in the destructor.

    --*/
    void Preserve();

    /*++
    
        Abstract:

            Writes bytes to the file.

        Parameters:

            buf - the buffer to write.
            cbIndex - index into the buffer to start the write.
            cbCount - the number of bytes to write.
            cbWritten - on success, receives the number of bytes written.

        Returns:

            S_OK - success. check cbWritten to see if all the bytes have been written.
            other - error.
    --*/
    HRESULT Write(
        const CBuffer<BYTE>& buf,
        size_t cbIndex, 
        size_t cbCount,
        OUT size_t& cbWritten);

private:
    CHeapWString m_strPath;
    HANDLE m_hFile;
    bool m_fPreserve;

    CTempFile(const CTempFile&) = delete;
    CTempFile& operator=(const CTempFile&) = delete;
};

