// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Query/GraphicsQuery.h"

namespace Engine
{
namespace Profiler
{
	using namespace Engine::Graphics;


	//
	// Function GPU Time Profiler
	//

	struct FunctionGpuProfiler
	{
	// variables
	private:
		// TODO: add other queries
		GraphicsQueryPtr		_gpuTimeQuery;
		String					_name;
		const Time<double>		_limit;		// normal time for function, if real time is greather, then log message marked as warning
		const char *			_file;
		const int				_line;


	// methods
	public:
		explicit
		FunctionGpuProfiler (const SubSystemsRef ss, const char *name, Time<double> limitSec) :
			_name(name), _limit(limitSec), _file(null), _line(0)
		{
			_gpuTimeQuery = GraphicsQuery::New( ss );
			_gpuTimeQuery->Create();
			_gpuTimeQuery->Begin( EQuery::TimeElapsed, 0 );
		}

		FunctionGpuProfiler (const SubSystemsRef ss, const char *name, const char *func, const char *file, int line, Time<double> limitSec) :
			_file(file), _line(line), _limit(limitSec)
		{
			_gpuTimeQuery = GraphicsQuery::New( ss );
			_gpuTimeQuery->Create();
			_gpuTimeQuery->Begin( EQuery::TimeElapsed, 0 );

			_name << "GPU profiler: " << name << ", function: " << func;
		}

		~FunctionGpuProfiler ()
		{
			_gpuTimeQuery->End();

			const Time<double>	dt = Time<double>().FromNanoSeconds( (double)_gpuTimeQuery->GetResult() );
			
			_name << "; TIME: ";
			StringUtils::TimeToStr( _name, dt.Seconds() );
			
			LOG_EXT( _name.cstr(), dt < _limit ? ELog::Debug : ELog::Warning, _file, _line );
		}
	};


# ifdef OP_ENABLE_PROFILING
	
	// name, limit
#	define GPUTIME_PROFILER( ... ) \
		Engine::Profiler::FunctionGpuProfiler \
				AUXDEF_UNITE_RAW( __funcGpuProf, __COUNTER__ ) ( \
					SubSystems(), \
					AUXDEF_GETRAW( AUXDEF_GET_FIRST( __VA_ARGS__, unknown ) ), \
					GX_FUNCTION_NAME, \
					__FILE__, \
					__LINE__, \
					Time<double>( AUXDEF_GETRAW( AUXDEF_GET_SECOND( __VA__ARGS__, 1.0e30 ) ) ) \
				)

# else

#	define GPUTIME_PROFILER( ... )

# endif	// OP_ENABLE_PROFILING


}	// Profiler
}	// Engine