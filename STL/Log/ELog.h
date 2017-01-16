// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

namespace GX_STL
{

	//
	// Log Message Type
	//

	struct ELog
	{
		enum type
		{
			Debug = 0,
			Info,
			Warning,
			Error,
			Fatal,
			_SrcFile,
			_COUNT,

			// for CHECK_* macro
			_DefError	= Error,
			_DefWarning	= Warning,

			// flags
			SpoilerFlag		= 1 << 8,					// spoiler closed by default
			OpenSpoilerFlag	= SpoilerFlag | (1 << 9),	// spoiler opened by default
			_FlagsMask		= 0xF,
		};

		static bool IsError (type e)
		{
			return (e & _FlagsMask) > Info;
		}

		inline friend ELog::type operator | (ELog::type left, ELog::type right)		{ return ELog::type(int(left) | int(right)); }
		inline friend ELog::type operator & (ELog::type left, ELog::type right)		{ return ELog::type(int(left) & int(right)); }
	};


	extern void __write_to_log (const char *msg, ELog::type type, const char *file, int line);

}	// GX_STL
