// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2Enums.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{

	class   CL2ComputeEngine;
	typedef CL2ComputeEngine		BaseComputeEngine;


	//
	// Compute Engine
	//

	class CL2ComputeEngine : public BaseObject
	{
	// types
	private:
		static const uint	MIN_VERSION = COMPUTE_API_OPENCL;


	// variables
	private:
		cl::cl_platform_id		_platform;
		cl::cl_device_id		_device;
		cl::cl_context			_context;
		cl::cl_command_queue	_queue;

		String					_buildOptions;

		ulong3					_maxThreads;
		ulong3					_maxLocalGroupSize;
		ulong					_maxInvocations;

		usize					_maxComputeUnits;

		Bytes<ulong>			_totalMemory;
		uint					_version;
		bool					_isShared;


	// methods
	public:
		explicit
		CL2ComputeEngine (const SubSystemsRef ss);
		~CL2ComputeEngine ();

		bool Initialize (StringCRef deviceName = Uninitialized);
		void Deinitialize ();
		
		void Synchronize ();
		void Barrier ();

		void SetProgramBuildOptions (StringCRef options);

		String const&			GetBuildOptions ()				const	{ return _buildOptions; }
		bool					IsShared ()						const	{ return _isShared; }
		uint					GetVersion ()					const	{ return _version; }
		
		ulong3 const&			GetMaxThreads ()				const	{ return _maxThreads; }
		ulong3					GetMaxWorkGroupCount ()			const	{ return GetMaxThreads() / GetMaxLocalGroupSize(); }
		ulong3 const&			GetMaxLocalGroupSize ()			const	{ return _maxLocalGroupSize; }
		ulong					GetMaxLocalGroupInvocations ()	const	{ return _maxInvocations; }

		usize					GetComputeUnitsCount ()			const	{ return _maxComputeUnits; }

		Bytes<ulong>			GetTotalMemory ()				const	{ return _totalMemory; }
		Bytes<ulong>			GetAvailableMemory ()			const;

		cl::cl_platform_id		GetPlatform ()					const	{ return _platform; }
		cl::cl_device_id		GetDevice ()					const	{ return _device; }
		cl::cl_context			GetContext ()					const	{ return _context; }
		cl::cl_command_queue	GetCommandQueue ()				const	{ return _queue; }


	private:
		bool _ChooseDevice (StringCRef deviceName);
		bool _CreateContext ();
		void _WriteInfo ();

		bool _CreateQueue ();
		void _DeleteQueue ();

		bool _CheckVersion ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
