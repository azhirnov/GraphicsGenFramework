// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

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
		GraphicsQueryPtr	_gpuTimeQuery;
		String				_name;
		const TimeD			_limit;		// normal time for function, if real time is greather, then log message marked as warning
		const char *		_file;
		const int			_line;


	// methods
	public:
		explicit
		FunctionGpuProfiler (const SubSystemsRef ss, const char *name, TimeD limitSec) :
			_name(name), _limit(limitSec), _file(null), _line(0)
		{
			_gpuTimeQuery = New<GraphicsQuery>( ss );
			_gpuTimeQuery->Create();
			_gpuTimeQuery->Begin( EQuery::TimeElapsed, 0 );
		}

		FunctionGpuProfiler (const SubSystemsRef ss, const char *name, const char *func, const char *file, int line, TimeD limitSec) :
			_file(file), _line(line), _limit(limitSec)
		{
			_gpuTimeQuery = New<GraphicsQuery>( ss );
			_gpuTimeQuery->Create();
			_gpuTimeQuery->Begin( EQuery::TimeElapsed, 0 );

			_name << "GPU profiler: " << name << ", function: " << func;
		}

		~FunctionGpuProfiler ()
		{
			_gpuTimeQuery->End();

			const TimeD	dt = TimeD::FromNanoSeconds( (double)_gpuTimeQuery->GetResult() );
			
			_name << "; TIME: " << ToString( dt );
			
			LOG( _name.cstr(), dt < _limit ? ELog::Debug : ELog::Warning, _file, _line );
		}
	};


# ifdef GX_ENABLE_PROFILING
	
	// name, limit
#	define GPUTIME_PROFILER( ... ) \
		Engine::Profiler::FunctionGpuProfiler \
				AUXDEF_UNITE_RAW( __funcGpuProf, __COUNTER__ ) ( \
					SubSystems(), \
					AUXDEF_GETRAW( AUXDEF_GETARG_0( ""__VA_ARGS__, "no name" ) ), \
					GX_FUNCTION_NAME, \
					__FILE__, \
					__LINE__, \
					TimeD( AUXDEF_GETRAW( AUXDEF_GETARG_1( __VA__ARGS__, 1.0e30 ) ) ) \
				)

# else

#	define GPUTIME_PROFILER( ... )

# endif	// GX_ENABLE_PROFILING


}	// Profiler
}	// Engine