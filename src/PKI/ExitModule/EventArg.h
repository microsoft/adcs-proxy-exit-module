#pragma once
/*++

    Copyright (C) Microsoft Corp. All rights reserved.

    File:

        EventArg.h

    Abstract:

        CEventArg and dervived class declarations.

    Authors:

        Jon Rowlett (jrowlett)

    History:
        20-Apr-2023 jrowlett Created.

--*/

/*++

    Abstract:

        Wrapper around event arguments of various types that exposes
        an abstract method to convert to string.
--*/
class CEventArg
{
public:
    virtual ~CEventArg() = default;

    /*++
    
        Abstract:

            Formats the value to a string and provides a reference to the string.

        Parameters:

            rpwszResult - on success, receives a pointer to the result string.

        Returns:

            S_OK - success.
            other - failure.

        Remarks:

            The class manages any memory allocated for the string. Callers should assume
            the string is no longer valid after the class instance is destroyed.

    --*/
    virtual HRESULT Format(OUT LPCWSTR& rpwszResult) = 0;

protected:
    CEventArg() = default;
private:
    CEventArg(const CEventArg&) = delete;
    CEventArg& operator=(const CEventArg&) = delete;
};

class CStringEventArg : public CEventArg
{
public:
    CStringEventArg(LPCWSTR pwszValue)
        : CEventArg(), m_pwszValue(pwszValue)
    {
    }

    virtual HRESULT Format(OUT LPCWSTR& rpwszResult);

private:
    LPCWSTR m_pwszValue;
};

template<typename T>
class CNumericEventArg : public CEventArg
{
public:
    CNumericEventArg(const T& varValue)
        : CEventArg(), m_varValue(varValue)
    {
    }

    virtual HRESULT Format(OUT LPCWSTR& rpwszResult)
    {
        rpwszResult = nullptr;
        HRESULT hr = ::StringCchPrintfW(
            m_bufResult.Get(),
            m_bufResult.GetLength(),
            s_pwszFormatString,
            m_varValue);
        if (SUCCEEDED(hr))
        {
            rpwszResult = m_bufResult.Get();
        }

        return hr;
    }

private:
    static const LPCWSTR s_pwszFormatString;
    const T m_varValue;
    CStaticBuffer<WCHAR, 32> m_bufResult;
};

class CErrorMessageEventArg : public CEventArg
{
public:
    CErrorMessageEventArg(HRESULT hr)
        : CEventArg(), m_hr(hr)
    {
    }

    virtual HRESULT Format(OUT LPCWSTR& rpwszResult);

private:
    HRESULT m_hr;
    CHeapBuffer<WCHAR> m_bufMessage;
};