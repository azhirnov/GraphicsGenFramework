// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/STL/Engine.STL.h"
#include "Debug.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;

typedef TDebugInstCounter<4>	Elem_t;
typedef TDebugInstCounter<2>	VElem_t;

typedef CircularQueue< Elem_t >	Queue_t;
typedef std::vector< VElem_t >	Vector_t;


static void CircularQueue_Test1 ()
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


static void CircularQueue_Test2 ()
{
	Queue_t		q;
	q.Resize( 12 );
	
	Queue_t		q2;
	q.Resize( 12 );

	q << RVREF( q2 );
}


static void CircularQueue_Test3 ()
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


static void CircularQueue_Test4 ()
{
	CircularQueue<int>	q;
	std::vector<int>	v;
	const usize			count = 100;

	for (usize i = 0; i < count; ++i)
	{
		q.PushBack( int(i) + 1000 );
		v.push_back( int(i) + 1000 );
	}
	
	for (usize i = 0; i < count; ++i)
	{
		q.PushFront( int(i) + 2000 );
		v.insert( v.begin(), int(i) + 2000 );
	}

	for (usize i = 0; i < count; ++i)
	{
		q.PushBack( int(i) + 3000 );
		v.push_back( int(i) + 3000 );
	}
	
	for (usize i = 0; i < count; ++i)
	{
		q.PushFront( int(i) + 4000 );
		v.insert( v.begin(), int(i) + 4000 );
	}

	Buffer<const int> part0;
	Buffer<const int> part1;

	q.GetParts( part0, part1 );

	ASSERT( part0.Count() + part1.Count() == q.Count() );

	ASSERT( (q == Buffer<int>( &v[0], v.size() )) );
}


extern void Test_Containers_CircularQueue ()
{
	Elem_t::ClearStatistic();

	CircularQueue_Test1();
	ASSERT( Elem_t::CheckStatistic() );
	Elem_t::ClearStatistic();

	CircularQueue_Test2();
	ASSERT( Elem_t::CheckStatistic() );
	Elem_t::ClearStatistic();

	CircularQueue_Test3();
	ASSERT( Elem_t::CheckStatistic() );
	Elem_t::ClearStatistic();

	CircularQueue_Test4();
}
