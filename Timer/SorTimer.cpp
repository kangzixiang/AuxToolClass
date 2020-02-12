//////////////////////////////////////////////////////////////////////////
//  SorTimer.cpp    $Revision:   1.0  $
//  ----------------------------------------------------------------------
//  $Created:   Feb 01 2020 13:42:13  $
//  $Modtime:   Feb 22 2020 18:42:48  $
//  $Author:   Kang  $
//  ----------------------------------------------------------------------
//  
//  ----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "SorTimer.h"

CSorTimer::CSorTimer()
          :m_frequency(0)
          ,m_resetTime(0)
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	Reset();
}

void CSorTimer::Reset()
{
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_resetTime);
}

float CSorTimer::GetSec(bool bReset/*=true*/)
{
	// Get current time
	volatile LONGLONG currentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

    LONGLONG diffTime=currentTime-m_resetTime;
    
	// Reset time
    if (bReset)
    {
        m_resetTime = currentTime;
    }

	// return time in seconds
	return (float)diffTime/(float)m_frequency;
}

float CSorTimer::GetMSec(bool bReset/*=true*/)
{
    return GetSec(bReset)*1000.0f;
}
