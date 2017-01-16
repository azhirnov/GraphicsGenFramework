// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaEnums.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
	class	CudaComputeProgram;
	typedef CudaComputeProgram		ComputeProgram;

	SHARED_POINTER( ComputeProgram );



	//
	// Compute Program
	//

	class CudaComputeProgram : public BaseObject
	{
		friend class CudaComputeFunction;

	// types
	private:
		typedef StaticString< char, 64 >		StString_t;
		typedef Set< StString_t >				GlobalVariables_t;

		struct KernelArgInfo
		{
		// variables
			StString_t	name;
			uint		index;

		// methods
			KernelArgInfo () : index(-1) {}
			explicit KernelArgInfo (StringCRef name, uint index = -1) : name(name), index(index) {}

			bool operator == (const KernelArgInfo &right) const	{ return name == right.name; }
			bool operator >  (const KernelArgInfo &right) const	{ return name >  right.name; }
			bool operator <  (const KernelArgInfo &right) const	{ return name <  right.name; }
		};

		typedef Set< KernelArgInfo >	KernelArgs_t;

		struct KernelInfo
		{
		// variables
			StString_t				name;
			mutable KernelArgs_t	args;

		// methods
			KernelInfo () {}

			explicit
			KernelInfo (StringCRef name) : name(name) {}

			bool operator == (const KernelInfo &right) const	{ return name == right.name; }
			bool operator >  (const KernelInfo &right) const	{ return name >  right.name; }
			bool operator <  (const KernelInfo &right) const	{ return name <  right.name; }
		};
		
		typedef Set< KernelInfo >		Kernels_t;


	// variables
	private:
		Kernels_t			_kernels;
		GlobalVariables_t	_globalVars;
		cuda::CUmodule		_module;


	// methods
	protected:
		explicit
		CudaComputeProgram (const SubSystemsRef ss);
		~CudaComputeProgram ();

	public:
		bool Create (StringCRef filename);


		bool			IsCreated ()	const	{ return _module != null; }
		cuda::CUmodule	Id ()			const	{ return _module; }


		static ComputeProgramPtr New (const SubSystemsRef ss);


	// access from ComputeFunction class
	protected:
		bool _GetKernel (StringCRef name, OUT Ptr<const KernelInfo> &kernel, OUT uint &numArgs) const;
		bool _HasKernelArg (Ptr<const KernelInfo> kernel, const StString_t &name, OUT uint &index) const;
		bool _HasGlobal (const StString_t &name) const;


	// hidden
	private:
		bool _LoadFile (OUT String &asmData, StringCRef filename);

		bool _CreateLinker (OUT cuda::CUlinkState &linkState, OUT float &wallTime, OUT String &errorLog, OUT String &infoLog);
		void _DeleteLinker (INOUT cuda::CUlinkState &linkState, float wallTime, String &errorLog, String &infoLog);

		bool _Link (const String &asmData, cuda::CUlinkState linkState, StringCRef name);
		bool _Compile (cuda::CUlinkState linkState);

		void _ParseKernels (const String &asmData);
		void _ParseGlobalVars (const String &asmData);

		void _Delete ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
