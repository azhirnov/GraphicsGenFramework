// Copyright © 2014  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

namespace Engine
{
namespace Profiler
{
	using namespace Engine::Base;


	//
	// Function CPU Time Profiler
	//

	struct FunctionCpuProfiler
	{
	// variables
	private:
		TimeProfilerD	_time;
		String			_name;
		const TimeD		_limit;		// normal time for function, if real time is greather, then log message marked as warning
		const char *	_file;
		const int		_line;


	// methods
	public:
		explicit
		FunctionCpuProfiler (const char *name, TimeD limitSec) : _name(name), _limit(limitSec), _file(null), _line(0)
		{
			_time.Start();
		}

		FunctionCpuProfiler (const char *name, const char *func, const char *file, int line, TimeD limitSec) :
			_file(file), _line(line), _limit(limitSec)
		{
			_name << "CPU profiler: " << name << ", function: " << func;

			_time.Start();
		}

		~FunctionCpuProfiler ()
		{
			const TimeD	dt = _time.GetTimeDelta();
			
			_name << "; TIME: " << ToString( dt );

			LOG( _name.cstr(), dt < _limit ? ELog::Debug : ELog::Warning, _file, _line );
		}
	};


# ifdef GX_ENABLE_PROFILING
	
	// name, limit
#	define CPUTIME_PROFILER( ... ) \
		Engine::Profiler::FunctionCpuProfiler \
				AUXDEF_UNITE_RAW( __funcCpuProf, __COUNTER__ ) ( \
					AUXDEF_GETRAW( AUXDEF_GETARG_0( __VA_ARGS__, unknown ) ), \
					GX_FUNCTION_NAME, \
					__FILE__, \
					__LINE__, \
					TimeD( AUXDEF_GETRAW( AUXDEF_GETARG_1( __VA__ARGS__, 1.0e30 ) ) ) \
				)

# else

#	define CPUTIME_PROFILER( ... )

# endif	// GX_ENABLE_PROFILING


}	// Profiler
}	// Engine