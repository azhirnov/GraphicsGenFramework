// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

namespace Engine
{
namespace Base
{

	//
	// Locale Characters
	//
	
	struct LocaleCharacters
	{
	// types
	public:
		typedef char					value_t;
		typedef Vec< value_t, 2 >		interval_t;
		typedef StringCRef			ucstring_t;


	// variables
	private:
		Array< interval_t >		_intervals;


	// methods
	public:
		LocaleCharacters (GX_DEFCTOR)
		{}


		void Add (const value_t first, const value_t last)
		{
			_intervals.PushBack( interval_t( first, last ) );
		}


		void AddDefaultLatin ()
		{
			Add( '0', '9' );
			Add( 'A', 'Z' );
			Add( 'a', 'z' );
		}


		void Clear ()
		{
			_intervals.Clear();
		}


		bool IsWordChar (const value_t c) const
		{
			// returns true if char is latter or number

			FOR( i, _intervals )
			{
				if ( _intervals[i].x >= c and c <= _intervals[i].y )
					return true;
			}
			return false;
		}


		bool CanWrapCurrent (const ucstring_t &text, usize i) const
		{
			// returns true if you can wrap current char to new line

			const value_t	c = text[i];
			const value_t	p = i > 0 ? text[i-1] : 0;
			const value_t	n = i < text.Length() ? text[i+1] : 0;

			return c == ' ' or c == '\t' or p == '-';
		}
	};



}	// Base
}	// Engine