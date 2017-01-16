// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

namespace Engine
{
namespace Base
{
	struct Iso2Locale;
	struct Iso3Locale;


	//
	// Iso3 Locale
	//

	struct Iso3Locale
	{
	// variables
	private:
		StaticArray< char, 4 >		_name;

	// methods
	public:
		Iso3Locale (GX_DEFCTOR)
		{}

		explicit
		Iso3Locale (StringCRef name)
		{
			Set( name );
		}

		explicit
		Iso3Locale (const Iso2Locale &loc);


		void Set (StringCRef name)
		{
			_name.Copy( Buffer<const char>( name ) );
		}

		StringCRef Get () const
		{
			return StringCRef( _name.ptr(), _name.LastIndex() );
		}

		bool operator == (const Iso3Locale &right) const	{ return Get() == right.Get(); }
		bool operator != (const Iso3Locale &right) const	{ return Get() != right.Get(); }

		bool operator >  (const Iso3Locale &right) const	{ return Get() >  right.Get(); }
		bool operator <  (const Iso3Locale &right) const	{ return Get() <  right.Get(); }
		
		bool operator >= (const Iso3Locale &right) const	{ return Get() >= right.Get(); }
		bool operator <= (const Iso3Locale &right) const	{ return Get() <= right.Get(); }
	};



	//
	// Iso2 Locale
	//

	struct Iso2Locale
	{
	// variables
	private:
		StaticArray< char, 3 >		_name;

	// methods
	public:
		Iso2Locale (GX_DEFCTOR)
		{}

		explicit
		Iso2Locale (StringCRef name)
		{
			Set( name );
		}
		
		explicit
		Iso2Locale (const Iso3Locale &loc);


		void Set (StringCRef name)
		{
			_name.Copy( Buffer<const char>( name ) );
		}

		StringCRef Get () const
		{
			return StringCRef( _name.ptr(), _name.LastIndex() );
		}

		bool operator == (const Iso2Locale &right) const	{ return Get() == right.Get(); }
		bool operator != (const Iso2Locale &right) const	{ return Get() != right.Get(); }

		bool operator >  (const Iso2Locale &right) const	{ return Get() >  right.Get(); }
		bool operator <  (const Iso2Locale &right) const	{ return Get() <  right.Get(); }
		
		bool operator >= (const Iso2Locale &right) const	{ return Get() >= right.Get(); }
		bool operator <= (const Iso2Locale &right) const	{ return Get() <= right.Get(); }
	};



	//
	// Iso Locale
	//

	struct IsoLocale
	{
	// types
	private:
		struct _IsoPair
		{
			Iso2Locale	iso2;
			Iso3Locale	iso3;
			uint		code;
			
			_IsoPair ()
			{}

			void Set (StringCRef iso2, StringCRef iso3, uint code)
			{
				this->iso2.Set( iso2 );
				this->iso3.Set( iso3 );
				this->code = code;
			}
		};

		typedef StaticArray< _IsoPair, 239 >	iso_pair_array_t;


	// variables
	private:
		iso_pair_array_t	_isoPairs;


	// methods
	public:
		IsoLocale (GX_DEFCTOR);
		
		static Iso3Locale	Iso2ToIso3 (const Iso2Locale &iso2);
		static Iso2Locale	Iso3ToIso2 (const Iso3Locale &iso3);

		static uint			GetCode (const Iso2Locale &iso2);
		static uint			GetCode (const Iso3Locale &iso3);

	private:
		static IsoLocale *	_Instance ();
	};
	
	
}	// Base
}	// Engine
