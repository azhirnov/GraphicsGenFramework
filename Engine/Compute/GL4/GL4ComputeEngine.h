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
		ulong3			_maxWorkGroupCount;
		ulong3			_maxLocalGroupSize;
		ulong			_maxInvocations;
		bool			_variableGroupSizeSupported;


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

		bool			IsVariableGroupSizeSupported ()	const	{ return _variableGroupSizeSupported; }

		ulong3			GetMaxThreads ()				const	{ return GetMaxWorkGroupCount() * GetMaxLocalGroupSize(); }
		ulong3 const&	GetMaxWorkGroupCount ()			const	{ return _maxWorkGroupCount; }
		ulong3 const&	GetMaxLocalGroupSize ()			const	{ return _maxLocalGroupSize; }
		ulong			GetMaxLocalGroupInvocations ()	const	{ return _maxInvocations; }

	private:
		void _CalcTotalMemory ();
		void _GetLocalGroupSize ();
		void _GetMaxWorkGroups ();
		void _CheckIsVariableGroupSizeSupported ();

		void _PrintInfo ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
