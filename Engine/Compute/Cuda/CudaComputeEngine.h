// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaEnums.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{

	class   CudaComputeEngine;
	typedef CudaComputeEngine		BaseComputeEngine;


	//
	// Compute Engine
	//

	class CudaComputeEngine : public BaseObject
	{
	// types
	private:
		static const uint	MIN_VERSION = COMPUTE_API_CUDA;


	// variables
	private:
		cuda::CUdevice			_device;
		cuda::CUcontext			_context;
		cuda::CUstream			_stream;

		ulong					_totalMemory;
		uint					_version;
		bool					_isShared;


	// methods
	public:
		explicit
		CudaComputeEngine (const SubSystemsRef ss);
		~CudaComputeEngine ();

		bool Initialize (StringCRef deviceName = StringCRef());
		void Deinitialize ();

		void Finish ();
		void Barrier ();

		cuda::CUstream	GetStream ()		const	{ return _stream; }

		bool			IsShared ()			const	{ return _isShared; }
		uint			GetVersion ()		const	{ return _version; }
		Bytes<ulong>			GetTotalMemory ()	const	{ return Bytes<ulong>( _totalMemory ); }
		Bytes<ulong>			GetFreeMemory ()	const;


	private:
		bool _ChooseDevice (StringCRef deviceName);
		bool _CreateContext ();
		bool _CreateQueue ();
		void _CheckSharedGraphicsDevices ();
		void _WriteInfo ();
		bool _CheckVersion ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
