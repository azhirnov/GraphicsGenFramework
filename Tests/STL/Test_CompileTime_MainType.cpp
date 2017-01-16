// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::CompileTime;


template <typename T>
const char * TypeNameOf (const T&) {
	return TypeDescriptor::template GetTypeInfo<T>::Name();
}


extern void Test_CompileTime_MainType ()
{
	STATIC_ASSERT( (IsSameTypes< int,		MainTypeStd< byte, byte > >) );
	STATIC_ASSERT( (IsSameTypes< int,		MainTypeStd< byte, ubyte > >) );
	STATIC_ASSERT( (IsSameTypes< int,		MainTypeStd< short, byte > >) );
	STATIC_ASSERT( (IsSameTypes< int,		MainTypeStd< byte, int > >) );
	STATIC_ASSERT( (IsSameTypes< uint,		MainTypeStd< uint, byte > >) );
	STATIC_ASSERT( (IsSameTypes< ilong,		MainTypeStd< byte, ilong > >) );
	STATIC_ASSERT( (IsSameTypes< ulong,		MainTypeStd< ulong, byte > >) );
	STATIC_ASSERT( (IsSameTypes< float,		MainTypeStd< byte, float > >) );
	STATIC_ASSERT( (IsSameTypes< double,	MainTypeStd< double, byte > >) );
	STATIC_ASSERT( (IsSameTypes< double,	MainTypeStd< float, double > >) );
	STATIC_ASSERT( (IsSameTypes< double,	MainTypeStd< double, float > >) );
	STATIC_ASSERT( (IsSameTypes< uint,		MainTypeStd< int, uint > >) );
	STATIC_ASSERT( (IsSameTypes< ulong,		MainTypeStd< ulong, ilong > >) );

	STATIC_ASSERT( (IsSameTypes< byte,		MainType< byte, byte > >) );
	STATIC_ASSERT( (IsSameTypes< ubyte,		MainType< byte, ubyte > >) );
	STATIC_ASSERT( (IsSameTypes< short,		MainType< byte, short > >) );
	STATIC_ASSERT( (IsSameTypes< ushort,	MainType< byte, ushort > >) );
	STATIC_ASSERT( (IsSameTypes< int,		MainType< byte, int > >) );
	STATIC_ASSERT( (IsSameTypes< uint,		MainType< byte, uint > >) );
	STATIC_ASSERT( (IsSameTypes< ilong,		MainType< byte, ilong > >) );
	STATIC_ASSERT( (IsSameTypes< float,		MainType< byte, float > >) );
	STATIC_ASSERT( (IsSameTypes< uint,		MainType< uint, int > >) );
	STATIC_ASSERT( (IsSameTypes< uint,		MainType< int, uint > >) );
	STATIC_ASSERT( (IsSameTypes< ulong,		MainType< ulong, ilong > >) );
	STATIC_ASSERT( (IsSameTypes< ulong,		MainType< ilong, ulong > >) );
	STATIC_ASSERT( (IsSameTypes< double,	MainType< float, double > >) );
	STATIC_ASSERT( (IsSameTypes< double,	MainType< double, float > >) );
	STATIC_ASSERT( (IsSameTypes< float,		MainType< Fixed16_t, int > >) );
	STATIC_ASSERT( (IsSameTypes< float,		MainType< half, byte > >) );
	STATIC_ASSERT( (IsSameTypes< double,	MainType< half, Fixed64_t > >) );

	/* to check C++ spec
	auto a = int(1) * uint(2);
	auto b = float(1) * ilong(2);
	auto c = ulong(1) * float(2);
	auto d = ilong(1) * ubyte(2);
	auto e = byte(1) * ubyte(2);
	auto f = byte(1) * byte(2);

	String str;

	str << "int * uint = " << TypeNameOf(a) << '\n';
	str << "float * ilong = " << TypeNameOf(b) << '\n';
	str << "ulong * float = " << TypeNameOf(c) << '\n';
	str << "ilong * ubyte = " << TypeNameOf(d) << '\n';
	str << "byte * ubyte = " << TypeNameOf(e) << '\n';
	str << "byte * byte = " << TypeNameOf(f) << '\n';

	WARNING( str.cstr() );
	//*/
}
