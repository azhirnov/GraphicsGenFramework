// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2Enums.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{
	class   CL2ComputeProgram;
	typedef CL2ComputeProgram	ComputeProgram;
	SHARED_POINTER( ComputeProgram );



	//
	// Compute Program
	//

	class CL2ComputeProgram : public BaseObject
	{
	// variables
	private:
		cl::cl_program		_id;


	// methods
	protected:
		explicit
		CL2ComputeProgram (const SubSystemsRef ss);
		~CL2ComputeProgram ();

	public:
		bool Create (StringCRef filename, EProgramUnitFlags::type compilationFlags);

		bool			IsCreated ()	const	{ return _id != null; }
		cl::cl_program	Id ()			const	{ return _id; }


		static ComputeProgramPtr New (const SubSystemsRef ss);


	private:
		bool _SetSource (StringCRef source);
		bool _Compile () const;

		bool _RecursiveParse (INOUT Array<String> &tmp, INOUT Array<usize2> &source,
							  INOUT HashSet<String> &includedFiles, StringCRef srcFilename) const;

		bool _Load (StringCRef filename, OUT Array<String> &tmp, INOUT String &source) const;
		bool _ReadFile (StringCRef filename, OUT String &src) const;
		bool _ParseString (StringCRef src, INOUT usize &pos, OUT StringCRef &filename) const;

		void _Delete ();

		static const char *	_DefaultExtension ()	{ return "cl"; }
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
