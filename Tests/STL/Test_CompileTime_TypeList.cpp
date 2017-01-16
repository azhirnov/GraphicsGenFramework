// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


template <usize Index, typename Type, typename PrevFuncResult, typename Typelist>
struct BuildTypeList
{
	STATIC_ASSERT(( CompileTime::IsSameTypes< Type, typename Typelist::template Get<Index> > ));

	typedef CompileTime::TypeList< Type, PrevFuncResult >	result;
};


extern void Test_CompileTime_TypeList ()
{
	typedef CompileTime::TypeListFrom< ubyte, short, uint, ilong, float, half, double >		typeList0;
	STATIC_ASSERT( typeList0::Length == 7 );
	STATIC_ASSERT( (typeList0::IsType<0, ubyte >) );
	STATIC_ASSERT( (typeList0::IsType<1, short >) );
	STATIC_ASSERT( (typeList0::IsType<3, ilong >) );
	STATIC_ASSERT( (typeList0::IsType<6, double >) );

	typedef typeList0::PushBack< int >							typeList1;
	STATIC_ASSERT( typeList1::Length == 8 );
	STATIC_ASSERT(( CompileTime::IsSameTypes< typeList1::Back, int > ));

	typedef typeList0::PushFront< float8u_t >					typeList2;
	STATIC_ASSERT( typeList2::Length == 8 );
	STATIC_ASSERT(( CompileTime::IsSameTypes< typeList2::Front, float8u_t > ));

	typedef typeList0::PopBack									typeList3;
	STATIC_ASSERT( typeList3::Length == 6 );
	STATIC_ASSERT(( CompileTime::IsSameTypes< typeList3::Back, half > ));

	typedef typeList0::PopFront									typeList4;
	STATIC_ASSERT( typeList4::Length == 6 );
	STATIC_ASSERT(( CompileTime::IsSameTypes< typeList4::Front, short > ));

	typedef typeList0::PushBack< short >::PushBack< float >		typeList5;
	STATIC_ASSERT( typeList5::FirstIndexOf< short > == 1 );
	STATIC_ASSERT( typeList5::LastIndexOf< short > == 7 );
	
	STATIC_ASSERT( typeList0::Equal< typeList0 > );
	STATIC_ASSERT( typeList0::MaxSizeOf() == 8 );

	typedef typeList0::ForEach< BuildTypeList >					typeList6;
	typedef typeList6::ForEach< BuildTypeList >					typeList7;
	STATIC_ASSERT( typeList0::Equal< typeList7 > );
	
	typedef typeList0::ReverseForEach< BuildTypeList >			typeList8;
	STATIC_ASSERT( typeList0::Equal< typeList8 > );

	typedef typeList0::SubList< 1, 2 >							typeList9;
	typedef CompileTime::TypeListFrom< short, uint >			typeList10;
	STATIC_ASSERT( typeList9::Equal< typeList10 > );

	typedef typeList10::Append< CompileTime::TypeListFrom< ulong > >	typeList11;
	STATIC_ASSERT( typeList11::Length == 3 );
	STATIC_ASSERT( typeList11::IndexOf< short > == 0 );
	STATIC_ASSERT( typeList11::IndexOf< uint > == 1 );
	STATIC_ASSERT( typeList11::IndexOf< ulong > == 2 );

	typedef CompileTime::ValueListFrom< int, 0, 1, 2, 3 >		valueList0;
	STATIC_ASSERT( valueList0::Get<0>::value == 0 );
	STATIC_ASSERT( valueList0::Get<1>::value == 1 );
	STATIC_ASSERT( valueList0::Get<2>::value == 2 );
	STATIC_ASSERT( valueList0::Get<3>::value == 3 );
}
