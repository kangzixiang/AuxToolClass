//////////////////////////////////////////////////////////////////////////
//  SorTimer.h    $Revision:   1.0  $
//  ----------------------------------------------------------------------
//  $Created:   Feb 01 2020 10:18:41  $
//  $Modtime:   Feb 21 2020 16:02:42  $
//  $Author:   Kang  $
//  ----------------------------------------------------------------------
//  
//  ----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "windows.h"

#include "SorBaseType.h"

class SOLAR_BASE_DLL_SPEC CSorTimer
{
protected:
	LONGLONG m_frequency;
	LONGLONG m_resetTime;

public:
	CSorTimer();
	
	void Reset();
	
	float GetSec(bool bReset=true);
	float GetMSec(bool bReset=true);
};
