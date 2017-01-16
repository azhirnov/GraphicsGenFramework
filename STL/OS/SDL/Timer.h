// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OS_SDL.h"

#ifdef PLATFORM_SDL

#include "../Base/Date.h"
#include "../Windows/Timer.h"
#include "../Posix/Timer.h"

namespace GX_STL
{
namespace _platform_
{
	
	//
	// Perfomance Timer
	//

	struct PerfTimer
	{
	private:
		ulong	_freq;

	public:
		PerfTimer ()
		{
			_freq = ::SDL_GetPerformanceFrequency();
		}

		// seconds
		Time<double> GetTime () const
		{
			return Time<double>().FromSeconds( double(::SDL_GetPerformanceCounter()) / _freq );
		}

		// microseconds
		Time<ulong> GetTimeMicroSec () const
		{
			return Time<ulong>().FromMicroSeconds( ulong( GetTime() * 1.0e6 ) );
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


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_SDL