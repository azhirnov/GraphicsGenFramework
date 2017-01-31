// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

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
		BytesU	_ram;
		BytesU	_vram;

	public:
		ResourceMemStat ()
		{}

		BytesU	GetRamSize ()	const	{ return _ram; }
		BytesU	GetVRamSize ()	const	{ return _vram; }

		void Set (BytesU ram, BytesU vram)
		{
			_ram	= ram;
			_vram	= vram;
		}

		void Add (BytesU ram, BytesU vram)
		{
			_ram	+= ram;
			_vram	+= vram;
		}

		void Clear ()
		{
			_ram = _vram = BytesU();
		}

		void Swap (ResourceMemStat &other)
		{
			SwapMembers( *this, other, &ResourceMemStat::_ram );
			SwapMembers( *this, other, &ResourceMemStat::_vram );
		}
	};

	
}	// Base
}	// Engine
