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
		TimeProfiler<double>	_time;
		String					_name;
		const Time<double>		_limit;		// normal time for function, if real time is greather, then log message marked as warning
		const char *			_file;
		const int				_line;


	// methods
	public:
		explicit
		FunctionCpuProfiler (const char *name, Time<double> limitSec) : _name(name), _limit(limitSec), _file(null), _line(0)
		{
			_time.Start();
		}

		FunctionCpuProfiler (const char *name, const char *func, const char *file, int line, Time<double> limitSec) :
			_file(file), _line(line), _limit(limitSec)
		{
			_name << "CPU profiler: " << name << ", function: " << func;

			_time.Start();
		}

		~FunctionCpuProfiler ()
		{
			const Time<double>	dt = _time.GetTimeDelta();
			
			_name << "; TIME: ";
			StringUtils::TimeToStr( _name, dt.Seconds() );

			LOG_EXT( _name.cstr(), dt < _limit ? ELog::Debug : ELog::Warning, _file, _line );
		}
	};


# ifdef OP_ENABLE_PROFILING
	
	// name, limit
#	define CPUTIME_PROFILER( ... ) \
		Engine::Profiler::FunctionCpuProfiler \
				AUXDEF_UNITE_RAW( __funcCpuProf, __COUNTER__ ) ( \
					AUXDEF_GETRAW( AUXDEF_GET_FIRST( __VA_ARGS__, unknown ) ), \
					GX_FUNCTION_NAME, \
					__FILE__, \
					__LINE__, \
					Time<double>( AUXDEF_GETRAW( AUXDEF_GET_SECOND( __VA__ARGS__, 1.0e30 ) ) ) \
				)

# else

#	define CPUTIME_PROFILER( ... )

# endif	// OP_ENABLE_PROFILING


}	// Profiler
}	// Engine