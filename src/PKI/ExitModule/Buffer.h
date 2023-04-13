#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        Buffer.h

    Abstract:

        Buffer management templates.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        10-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:
    
        Common Buffer class.

    Remarks:

        Use this instead of passing around arrays or raw pointers with a size.
        Derived classes do memory management.
--*/
template<typename T>
class CBuffer
{
public:
    inline const T* Get() const 
    { 
        return m_p; 
    }

    inline T* Get()
    {
        return m_p;
    }

    inline size_t GetLength() const
    {
        return m_cLength;
    }

    inline size_t GetSize() const
    {
        return m_cLength * sizeof(T);
    }

protected:
    inline CBuffer()
        : m_p(nullptr), m_cLength(0)
    {
    }

    inline CBuffer(T* p, size_t cLength)
        : m_p(p), m_cLength(cLength)
    {
    }

    T* m_p;
    size_t m_cLength;

private:
    CBuffer(const CBuffer&) = delete;
    CBuffer& operator=(const CBuffer&) = delete;
};

/*++

    Abstract:

        A fixed size, statically allocated buffer.

    Remarks:

        Use this for either member variables or stack allocated buffers.
--*/
template<typename T, size_t N>
class CStaticBuffer : public CBuffer<T>
{
public:
    CStaticBuffer()
        : CBuffer<T>(m_Elements, N)
    {
    }

private:
    T m_Elements[N];
};

/*++

    Abstract:

        A reference buffer to an externally allocated buffer.

--*/
template<typename T>
class CRefBuffer : public CBuffer<T>
{
public:
    CRefBuffer(T* p, size_t cLength)
        : CBuffer(p, cLength)
    {
    }
};

/*++

    Abstract:

        A Heap allocated buffer.

    Remarks:

        The class manages heap memory for the buffer.
--*/
template<typename T>
class CHeapBuffer : public CBuffer<T>
{
public:
    CHeapBuffer()
        : CBuffer<T>()
    {
    }

    ~CHeapBuffer()
    {
        Clear();
    }

    /*++
    
        Abstract:

            Clears the buffer and frees memory.
    --*/
    void Clear()
    {
        if (m_p)
        {
            delete[] m_p;
            m_p = nullptr;
        }

        m_cLength = 0;
    }

    /*++
    
        Abstract:

            Allocates a buffer of a given number of elements.

        Parameters:

            cLength - the number of elements to allocate in the new buffer.

        Returns:

            true - success.
            false - the new buffer failed to get allocated.

        Remarks:

            It creates a new buffer and does not copy elements from the old buffer.
            If the new buffer cannot be allocated, there is no change to the existing buffer.

    --*/
    bool Alloc(size_t cLength)
    {
        T* pNew = new T[cLength];
        if (!pNew)
        {
            return false;
        }

        Clear();
        m_p = pNew;
        m_cLength = cLength;
        return true;
    }
};

/*++

    Abstract:

        Specialization of the heap buffer for null terminated WCHAR strings.

    Remarks:

        CStringW and CAtlStringW do not report errors well in low memory conditions.
--*/
class CHeapWString : public CHeapBuffer<WCHAR>
{
public:
    /*++
    
        Abstract:

            Copies the source BSTR into the string.

        Parameters:

            bstr - the source bstr to copy.

        Returns:

            S_OK - success.
            E_OUTOFMEMORY - out of memory allocating the string.
            other - internal error from ::StringCchCopy.
    --*/
    HRESULT Copy(const BSTR bstr)
    {
        if (bstr)
        {
            size_t cch = ::SysStringLen(bstr) + 1;
            if (!Alloc(cch))
            {
                return E_OUTOFMEMORY;
            }

            // include the trailing null in cch.
            return StringCchCopyW(m_p, cch, bstr);
        }
        else
        {
            Clear();
        }

        return S_OK;
    }
};