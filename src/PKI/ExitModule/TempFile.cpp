/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        TempFile.cpp

    Abstract:

        CTempFile class implementation.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        11-Apr-2023 jrowlett Created.

--*/
#include "pch.h"
#include "TempFile.h"

CTempFile::~CTempFile()
{
    Close();
    if (!m_fPreserve)
    {
        if (m_strPath.GetLength() > 0)
        {
            if (!::DeleteFileW(m_strPath.Get()))
            {
                HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
                ATLTRACE(
                    L"Failed to delete temp file [%s], hr=%x\n",
                    m_strPath.Get(),
                    hr);
            }
        }
    }
    else
    {
        ATLTRACE(L"Preserving temp file [%s] for debugging.\n", m_strPath.Get());
    }
}

HRESULT CTempFile::Create(LPCWSTR pwszPath)
{
    if (m_strPath.GetLength() > 0)
    {
        ATLTRACE(L"The temp file has been previously initialized.\n");
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    }

    Close();
    size_t cch = 0;
    HRESULT hr = ::StringCchLengthW(pwszPath, MAX_PATH + 1, &cch);
    if (FAILED(hr))
    {
        ATLTRACE(L"::StringCchLengthW failed, hr=%x\n", hr);
        return hr;
    }

    if (!m_strPath.Alloc(cch + 1))
    {
        ATLTRACE(L"Failed to alloc %d+1 wchars\n", cch);
        hr = E_OUTOFMEMORY;
        return hr;
    }

    hr = ::StringCchCopyW(m_strPath.Get(), m_strPath.GetLength(), pwszPath);
    if (FAILED(hr))
    {
        ATLTRACE(L":StringCchCopyW failed, hr=%x\n", hr);
        return hr;
    }

    //
    // NOTE: The Win32 APIs to get the temp file name already create the file,
    // so always recreate the file in this step.
    //
    m_hFile = ::CreateFileW(
        pwszPath,
        GENERIC_WRITE,
        0,			// dwShareMode
        NULL,		// lpSecurityAttributes
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);		// hTemplateFile
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"::CreateFileW(%s) failed, hr=%x\n", pwszPath, hr);
        m_strPath.Clear();
    }

    return hr;
}

void CTempFile::Close()
{
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

void CTempFile::Preserve()
{
    m_fPreserve = true;
}

HRESULT CTempFile::Write(
    const CBuffer<BYTE>& buf,
    size_t cbIndex,
    size_t cbCount,
    OUT size_t& cbWritten)
{
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        ATLTRACE(L"File not open.");
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    }

    DWORD nBytesWritten = 0;
    if (!::WriteFile(
        m_hFile,
        buf.Get() + cbIndex,
        (DWORD)cbCount,
        &nBytesWritten,
        NULL)) // lpOverlapped
    {
        HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
        ATLTRACE(L"::WriteFile failed, cbCount=%x, hr=%x\n", cbCount, hr);
        return hr;
    }

    cbWritten = nBytesWritten;

    return S_OK;
}