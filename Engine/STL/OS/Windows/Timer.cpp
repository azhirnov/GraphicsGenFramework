// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Timer.h"

#ifdef PLATFORM_WINDOWS

#include "WinHeader.h"

namespace GX_STL
{
namespace OS
{

# ifdef PLATFORM_WINDOWS_SHELL
	
/*
=================================================
	constructor
=================================================
*/
	PerfomanceTimer::PerfomanceTimer () :
		_liFreq( UninitializedT<LARGE_INTEGER>() )
	{
		QueryPerformanceFrequency( &_liFreq.Get<LARGE_INTEGER>() );
	}
		
/*
=================================================
	GetTime
=================================================
*/
	TimeD PerfomanceTimer::GetTime () const
	{
		LARGE_INTEGER	perf_counter;
		QueryPerformanceCounter( &perf_counter );
		return TimeD::FromSeconds( (double)perf_counter.QuadPart / (double)_liFreq.Get<LARGE_INTEGER>().QuadPart );
	}
		
/*
=================================================
	GetTimeMicroSec
=================================================
*/
	TimeL PerfomanceTimer::GetTimeMicroSec () const
	{
		LARGE_INTEGER	perf_counter;
		QueryPerformanceCounter( &perf_counter );
		return TimeL::FromMicroSeconds( (perf_counter.QuadPart * 1000000) / _liFreq.Get<LARGE_INTEGER>().QuadPart );
	}

# endif	// PLATFORM_WINDOWS_SHELL

//-----------------------------------------------------------------------------
	


/*
=================================================
	Now
=================================================
*/
	Date & Date::Now ()
	{
		SYSTEMTIME	 time = {0};
		GetLocalTime( &time );

		_year		= time.wYear;
		_month		= time.wMonth-1;								// 1..12 convert to 0..11
		_dayOfWeek	= time.wDayOfWeek == 0 ? 6 : time.wDayOfWeek-1;	// 0..6, Sun == 0 convert to mon..sun
		_dayOfMonth	= time.wDay-1;									// 1..31 convert to 0..30
		_dayOfYear	= _CalcDayOfYear( _year, _month, _dayOfMonth );
		
		_hour		= time.wHour;									// 0..23
		_minute		= time.wMinute;									// 0..59
		_second		= time.wSecond;									// 0..59
		_millis		= time.wMilliseconds;							// 0..999

		return *this;
	}

//-----------------------------------------------------------------------------


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS