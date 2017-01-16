// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Config
	//

	struct CoreTypesConfig
	{
		// string //
		static const uint		StringDefaultRadix				= 10;

		// memory container //
		// newSize = size * nominator / denominator + minSize
		static const uint		MemContainerResizingMinSize		= 16;
		static const uint		MemContainerResizingNominator	= 1;	// must be > 0
		static const uint		MemContainerResizingDenominator	= 2;	// must be > 0
	};



	//
	// Statistic
	//

	DEBUG_ONLY(
		struct CoreTypesStatistic
		{
			// buffered ptr //
			static usize	BufferedPtrMaxBufferSize;
		};
	)


}	// GXTypes
}	// GX_STL
