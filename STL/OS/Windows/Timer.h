// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"
#include "../Base/Date.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace _platform_
{
	
#ifdef PLATFORM_WINDOWS_SHELL

	//
	// Perfomance Timer
	//

	struct PerfTimer
	{
	private:
		LARGE_INTEGER	_liFreq;

	public:
		PerfTimer ()
		{
			QueryPerformanceFrequency( &_liFreq );
		}

		// seconds
		Time<double> GetTime () const
		{
			LARGE_INTEGER	perf_counter;
			QueryPerformanceCounter( &perf_counter );
			return Time<double>().FromSeconds( (double)perf_counter.QuadPart / (double)_liFreq.QuadPart );
		}

		// microseconds
		Time<ulong> GetTimeMicroSec () const
		{
			LARGE_INTEGER	perf_counter;
			QueryPerformanceCounter( &perf_counter );
			return Time<ulong>().FromMicroSeconds( (perf_counter.QuadPart * 1000000) / _liFreq.QuadPart );
		}

		template <typename T>
		T Get (const T&) const;
	};
	

	template <>
	inline Time<ulong>  PerfTimer::Get (const Time<ulong> &) const
	{
		return GetTimeMicroSec();
	}
	
	template <>
	inline Time<double>  PerfTimer::Get (const Time<double> &) const
	{
		return GetTime();
	}

#endif	// PLATFORM_WINDOWS_SHELL


	
	// Now
	inline Date & Date::Now ()
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


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS