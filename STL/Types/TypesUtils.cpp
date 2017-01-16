// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "../ux_stl.h"

#include "../Defines/PublicMacro.h"
#include "../CompileTime/CompileTimeChecks.h"
#include "../Math/Rand/Random.h"

namespace GX_STL
{
namespace GXMath
{

/*
=================================================
	InitializeAllRandomGenerators_Op
=================================================
*/
	struct InitializeAllRandomGenerators_Op
	{
		usize result;

		InitializeAllRandomGenerators_Op () : result(0)
		{}

		template <typename T, usize Index>
		void Process ()
		{
			result += (uint) GXMath::TRandomGenerator<T>::Initialize();
		}
	};

/*
=================================================
	InitializeAllRandomGenerators
=================================================
*/
	static bool InitializeAllRandomGenerators ()
	{
		typedef CompileTime::TypeListFrom< GXMath::DefaultRandomEngine, GXMath::PlatformRandomEngine >	engine_list;

		InitializeAllRandomGenerators_Op	op;
		engine_list::RuntimeForEach( op );

		return op.result & 1;
	}
	
/*
=================================================
	InitializeSTLMath
=================================================
*/
	void InitializeSTLMath ()
	{
		InitializeAllRandomGenerators();
	}

}	// GXMath
	
	
namespace GXTypes
{
	
DEBUG_ONLY(
	// buffered ptr //
	usize				CoreTypesStatistic::BufferedPtrMaxBufferSize = 0;
)

#if not defined( GX_THREADSAFE_STATIC_SUPPORTED )
	Mutex				SingletonMultiThread::_mutex;
#endif
	
DEBUG_ONLY(
	Set<Referenced*>	Referenced::_objectRefs;
	Mutex				Referenced::_mutex;
)

}	// GXTypes
}	// GX_STL
