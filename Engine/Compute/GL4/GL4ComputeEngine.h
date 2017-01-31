// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4Enums.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{

	class   GL4ComputeEngine;
	typedef GL4ComputeEngine		BaseComputeEngine;


	//
	// Compute Engine
	//

	class GL4ComputeEngine : public BaseObject
	{
	// variables
	private:
		Bytes<ulong>	_totalMemory;
		uint3			_maxWorkGroupCount;
		uint3			_maxLocalGroupSize;
		uint			_maxInvocations;


	// methods
	public:
		explicit
		GL4ComputeEngine (const SubSystemsRef ss);
		~GL4ComputeEngine ();

		bool Initialize (StringCRef deviceName = Uninitialized);
		void Deinitialize ();
		
		void Synchronize ();
		void Barrier ();

		bool			IsShared ()						const	{ return true; }

		Bytes<ulong>	GetTotalMemory ()				const	{ return _totalMemory; }
		Bytes<ulong>	GetAvailableMemory ()			const;

		uint3 const&	GetMaxWorkGroupSize ()			const	{ return _maxLocalGroupSize; }
		uint			GetMaxWorkGroupInvocations ()	const	{ return _maxInvocations; }

	private:
		void _CalcTotalMemory ();
		void _GetLocalGroupSize ();
		void _GetMaxWorkGroups ();
		void _PrintInfo ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
