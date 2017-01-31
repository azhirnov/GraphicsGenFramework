// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"
#include "Engine/STL/OS/Base/Date.h"

#ifdef PLATFORM_WINDOWS

namespace GX_STL
{
namespace OS
{
	
# ifdef PLATFORM_WINDOWS_SHELL

	//
	// Perfomance Timer
	//

	struct PerfomanceTimer
	{
	// types
	private:
		typedef HiddenOSTypeFrom< ulong >	Handle_t;	// LARGE_INTEGER


	// variables
	private:
		Handle_t	_liFreq;


	// methods
	public:
		PerfomanceTimer ();

		// seconds
		TimeD GetTime () const;

		// microseconds
		TimeL GetTimeMicroSec () const;

		template <typename T>
		T Get (const T&) const;
	};
	

	template <>
	inline TimeL  PerfomanceTimer::Get (const TimeL &) const
	{
		return GetTimeMicroSec();
	}
	
	template <>
	inline TimeD  PerfomanceTimer::Get (const TimeD &) const
	{
		return GetTime();
	}

# endif	// PLATFORM_WINDOWS_SHELL


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS