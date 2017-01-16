// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"
#include "Debug.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;

typedef TDebugInstCounter<3>	Elem_t;
typedef TDebugInstCounter<2>	VElem_t;

typedef Queue< Elem_t >			Queue_t;
typedef std::vector< VElem_t >	Vector_t;


void Queue_Test1 ()
{
	Queue_t		q;

	q.Resize( 12 );
	
	const Elem_t	e1;

	q << Elem_t() << Elem_t();
	q << e1;
	e1 >> q;
	q.PopBack();
	q.PopFront();
}

void Queue_Test2 ()
{
	Queue_t		q;
	q.Resize( 12 );
	
	Queue_t		q2;
	q.Resize( 12 );

	q << RVREF( q2 );

	//q << Array<Elem_t>();
	q << Buffer<const Elem_t>();
}

void Queue_Test3 ()
{
	VElem_t::ClearStatistic();

	Queue_t		q;
	Vector_t	vec;

	q.Resize( 12 );
	vec.resize( 12 );
	ASSERT( (q == Buffer<Elem_t>( (Elem_t *)&vec[0], vec.size() )) );
	
	q.Resize( 36 );
	vec.resize( 36 );
	ASSERT( (q == Buffer<Elem_t>( (Elem_t *)&vec[0], vec.size() )) );
	
	q.Resize( 6 );
	vec.resize( 6 );
	ASSERT( (q == Buffer<Elem_t>( (Elem_t *)&vec[0], vec.size() )) );
}

void Queue_Test4 ()
{
	Queue<int>			q;
	std::vector<int>	v;
	const usize			count = 900;

	for (usize i = 0; i < count; ++i)
	{
		q.PushBack( i + 1000 );
		v.push_back( i + 1000 );
	}
	
	for (usize i = 0; i < count; ++i)
	{
		q.PushFront( i + 2000 );
		v.insert( v.begin(), i + 2000 );
	}

	for (usize i = 0; i < count; ++i)
	{
		q.PushBack( i + 3000 );
		v.push_back( i + 3000 );
	}
	
	for (usize i = 0; i < count; ++i)
	{
		q.PushFront( i + 4000 );
		v.insert( v.begin(), i + 4000 );
	}

	ASSERT( (q == Buffer<int>( &v[0], v.size() )) );
}

extern void Test_Containers_Queue ()
{
	Elem_t::ClearStatistic();

	Queue_Test1();
	ASSERT( Elem_t::CheckStatistic() );
	Elem_t::ClearStatistic();

	Queue_Test2();
	ASSERT( Elem_t::CheckStatistic() );
	Elem_t::ClearStatistic();

	Queue_Test3();
	ASSERT( Elem_t::CheckStatistic() );
	Elem_t::ClearStatistic();

	Queue_Test4();
}
