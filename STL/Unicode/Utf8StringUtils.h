// Copyright � 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Containers/String.h"

namespace GX_STL
{
namespace GXDataBase
{
	using namespace GXTypes;



	//
	// UTF-8 String Utils
	//

	struct Utf8StringUtils
	{
		static void EncodeToWindows1251 (StringCRef src, OUT String &dst);

		static usize Length (const char *s)
		{
			usize len = 0;
			while (*s)
			{
				len += ( (*s++) & 0xc0 ) != 0x80;
			}
			return len;
		}
	};


}	// GXDataBase
}	// GX_STL
