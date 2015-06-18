#ifndef H__DXUTtimer
#define H__DXUTtimer

#include <Windows.h>
#include <assert.h>

//--------------------------------------------------------------------------------------
// A growable array
//--------------------------------------------------------------------------------------
template<typename TYPE> class CGrowableArray
{
public:
            CGrowableArray()
            {
                m_pData = NULL; m_nSize = 0; m_nMaxSize = 0;
            }
            CGrowableArray( const CGrowableArray <TYPE>& a )
            {
                for( int i = 0; i < a.m_nSize; i++ ) Add( a.m_pData[i] );
            }
            ~CGrowableArray()
            {
                RemoveAll();
            }

    const TYPE& operator[]( int nIndex ) const
    {
        return GetAt( nIndex );
    }
    TYPE& operator[]( int nIndex )
    {
        return GetAt( nIndex );
    }

    CGrowableArray& operator=( const CGrowableArray <TYPE>& a )
    {
        if( this == &a ) return *this; RemoveAll(); for( int i = 0; i < a.m_nSize;
                                                         i++ ) Add( a.m_pData[i] ); return *this;
    }

    HRESULT SetSize( int nNewMaxSize );
    HRESULT Add( const TYPE& value );
    HRESULT Insert( int nIndex, const TYPE& value );
    HRESULT SetAt( int nIndex, const TYPE& value );
    TYPE& GetAt( int nIndex ) const
    {
        assert( nIndex >= 0 && nIndex < m_nSize ); return m_pData[nIndex];
    }
    int     GetSize() const
    {
        return m_nSize;
    }
    TYPE* GetData()
    {
        return m_pData;
    }
    bool    Contains( const TYPE& value )
    {
        return ( -1 != IndexOf( value ) );
    }

    int     IndexOf( const TYPE& value )
    {
        return ( m_nSize > 0 ) ? IndexOf( value, 0, m_nSize ) : -1;
    }
    int     IndexOf( const TYPE& value, int iStart )
    {
        return IndexOf( value, iStart, m_nSize - iStart );
    }
    int     IndexOf( const TYPE& value, int nIndex, int nNumElements );

    int     LastIndexOf( const TYPE& value )
    {
        return ( m_nSize > 0 ) ? LastIndexOf( value, m_nSize - 1, m_nSize ) : -1;
    }
    int     LastIndexOf( const TYPE& value, int nIndex )
    {
        return LastIndexOf( value, nIndex, nIndex + 1 );
    }
    int     LastIndexOf( const TYPE& value, int nIndex, int nNumElements );

    HRESULT Remove( int nIndex );
    void    RemoveAll()
    {
        SetSize( 0 );
    }
    void    Reset()
    {
        m_nSize = 0;
    }

protected:
    TYPE* m_pData;      // the actual array of data
    int m_nSize;        // # of elements (upperBound - 1)
    int m_nMaxSize;     // max allocated

    HRESULT SetSizeInternal( int nNewMaxSize );  // This version doesn't call ctor or dtor.
};


//--------------------------------------------------------------------------------------
// Performs timer operations
// Use DXUTGetGlobalTimer() to get the global instance
//--------------------------------------------------------------------------------------
class CDXUTTimer
{
public:
                    CDXUTTimer();

    void            Reset(); // resets the timer
    void            Start(); // starts the timer
    void            Stop();  // stop (or pause) the timer
    void            Advance(); // advance the timer by 0.1 seconds
    double          GetAbsoluteTime(); // get the absolute system time
    double          GetTime(); // get the current time
    float           GetElapsedTime(); // get the time that elapsed between Get*ElapsedTime() calls
    void            GetTimeValues( double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime ); // get all time values at once
    bool            IsStopped(); // returns true if timer stopped

    // Limit the current thread to one processor (the current one). This ensures that timing code runs
    // on only one processor, and will not suffer any ill effects from power management.
    void            LimitThreadAffinityToCurrentProc();

protected:
    LARGE_INTEGER   GetAdjustedCurrentTime();

    bool m_bUsingQPF;
    bool m_bTimerStopped;
    LONGLONG m_llQPFTicksPerSec;

    LONGLONG m_llStopTime;
    LONGLONG m_llLastElapsedTime;
    LONGLONG m_llBaseTime;
};

CDXUTTimer*                 WINAPI DXUTGetGlobalTimer();

#endif