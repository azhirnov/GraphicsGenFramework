// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"

namespace Engine
{
namespace Base
{

	//
	// Resource Memory Statistic
	//

	struct ResourceMemStat
	{
	private:
		Bytes<usize>	_ram;
		Bytes<usize>	_vram;

	public:
		ResourceMemStat ()
		{}

		Bytes<usize>	GetRamSize ()	const	{ return _ram; }
		Bytes<usize>	GetVRamSize ()	const	{ return _vram; }

		void Set (Bytes<usize> ram, Bytes<usize> vram)
		{
			_ram	= ram;
			_vram	= vram;
		}

		void Add (Bytes<usize> ram, Bytes<usize> vram)
		{
			_ram	+= ram;
			_vram	+= vram;
		}

		void Swap (ResourceMemStat &other)
		{
			SwapMembers( *this, other, &ResourceMemStat::_ram );
			SwapMembers( *this, other, &ResourceMemStat::_vram );
		}
	};

	
}	// Base
}	// Engine
