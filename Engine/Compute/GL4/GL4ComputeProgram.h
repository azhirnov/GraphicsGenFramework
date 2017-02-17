// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	File format:

	[HEADER]

	// kernel
	#ifdef kernel_Func1

	#include "..." // include in header is NOT part of header!

	uniform layout(readonly, rgba32f) image2D tex;
	
	void kernel_Func1 (layout(readonly, rgba32f) image2D tex);

	void main ()
	{
		kernel_Func1( tex );
	}

	#endif

	// export function
	#if defined(export_Func) || defined(export_All)
	#endif


	[SOURCE]

	void kernel_Func1 (layout(readonly, rgba32f) image2D tex)
	{
	}
*/

#pragma once

#include "GL4Enums.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{
	class   GL4ComputeProgram;
	typedef GL4ComputeProgram		ComputeProgram;
	SHARED_POINTER( ComputeProgram );


	//
	// Compute Program
	//

	class GL4ComputeProgram : public BaseObject
	{
		friend class GL4ComputeFunction;

	// variables
	private:
		Array< ShaderID >				_shaders;
		String							_header;
		EShaderCompilationFlags::type	_flags;


	// methods
	public:
		explicit
		GL4ComputeProgram (const SubSystemsRef ss);
		~GL4ComputeProgram ();

		bool Create (StringCRef filename, EProgramUnitFlags::type compilationFlags);

		bool IsCreated () const;
		
		EProgramUnitFlags::type	GetFlags ()	const	{ return EProgramUnitFlags::type( _flags ); }

	protected:
		void _Delete ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
