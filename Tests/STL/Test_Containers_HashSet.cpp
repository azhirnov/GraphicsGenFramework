// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"
#include "Debug.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


struct Value
{
	int		val;

	Value () : val(0) {}
	Value (int x) : val(x) {}
	
	bool operator == (const Value &right) const		{ return val == right.val; }
	bool operator != (const Value &right) const		{ return val != right.val; }
	bool operator >  (const Value &right) const		{ return val >  right.val; }
	bool operator <  (const Value &right) const		{ return val <  right.val; }
};


template <>
struct Hash< Value >
{
	typedef usize	result_t;

	result_t operator () (const Value &) const
	{
		return 0;
	}
};


void HashSet_Test1 ()
{
	HashSet<Value>	s;

	s << Value(1) << Value(1);
	s << Value(2) << Value(2);

	ASSERT( s.Count() == 2 );
}


void HashSet_Test2 ()
{
	typedef TDebugInstCounter<5>	Elem1_t;
	typedef TDebugInstCounter<6>	Elem2_t;
	typedef TDebugInstCounter<7>	Elem3_t;

	Elem1_t::ClearStatistic();
	Elem2_t::ClearStatistic();
	Elem3_t::ClearStatistic();

	{
		HashSet<Elem1_t>		s;
		Array<Elem2_t>			a;
		std::vector<Elem3_t>	v;
	
		for (usize i = 0; i < 10; ++i)
		{
			s << Elem1_t(Random::Int<int>());
			a << Elem2_t(Random::Int<int>());
			v.push_back( Elem3_t(Random::Int<int>()) );
		}
	}

	ASSERT( Elem1_t::CheckStatistic() );
	ASSERT( Elem2_t::CheckStatistic() );
	ASSERT( Elem3_t::CheckStatistic() );
}


void HashSet_Test3 ()
{
	HashSet< String >	s;
	usize				idx;

	s << "111" << "222" << "333";

	ASSERT( s.FindIndex( StringCRef("222"), idx ) );
	
	HashSet< String >::const_iterator	iter;

	ASSERT( s.Find( "333", OUT iter ) );

	ASSERT( idx == 1 );
}


void MultiHashSet_Test1 ()
{
	MultiHashSet<Value>	m;
	
	m << Value(1) << Value(1);
	m << Value(2) << Value(2);

	ASSERT( m.Count() == 4 );
}


void HashMap_Test1 ()
{
	typedef Pair<Value, String>		P;

	MultiHashMap<Value, String>		m;
	
	m << P(Value(1), "1") << P(Value(1), "2");
	m << P(Value(2), "3") << P(Value(2), "4");

	ASSERT( m.Count() == 4 );
}


void HashMap_Test2 ()
{
	//typedef String					Str_t;
	typedef StaticString<32>		Str_t;

	typedef Pair<Str_t, int>		P;

	MultiHashMap<Str_t, int>		m;
	usize							idx;

	m << P("111", 0) << P("222", 1) << P("333", 2);
	m << P("444", 3) << P("555", 4) << P("666", 5);

	ASSERT( m.FindIndex( "222", OUT idx ) );
	
	MultiHashMap<Str_t, int>::iterator	iter;

	ASSERT( m.Find( "333", OUT iter ) );

	//ASSERT( idx->second == 1 );
}


void HashMap_Test3 ()
{
#if 0
	struct ValueWithUnion
	{
		String						s;
		Union<int, float, double>	u;
		uint						idx;

		ValueWithUnion (int i, String s) : s(s), idx(0) { u.Create( i ); }
		ValueWithUnion (float f, String s) : s(s), idx(1) { u.Create( f ); }
		ValueWithUnion (double d, String s) : s(s), idx(2) { u.Create( d ); }
	};
#else
	struct ValueWithUnion
	{
		uint						idx;

		ValueWithUnion (int i, String s) : idx(0) {}
		ValueWithUnion (float f, String s) : idx(1) {}
		ValueWithUnion (double d, String s) : idx(2) {}
	};
#endif

#if 1
	typedef String									StString_t;
	//typedef StaticString< 64 >						StString_t;
	typedef MultiHashMap< StString_t, ValueWithUnion >	Map_t;

	Map_t	m;

	m.Add( "111", ValueWithUnion( 1, "aaa" ) );
	m.Add( "222", ValueWithUnion( 2.0f, "bbb" ) );
	m.Add( "333", ValueWithUnion( 4.0, "ccc" ) );

	Map_t::iterator	iter;
	ASSERT( m.Find( "222", iter ) );
	//ASSERT( iter->second.u.Is<float>() );
	//ASSERT( iter->second.s == "bbb" );
#else

	typedef HashMap< int, ValueWithUnion >	Map_t;
	
	Map_t	m;

	m.Add( 111, ValueWithUnion( 1, "aaa" ) );
	m.Add( 222, ValueWithUnion( 2.0f, "bbb" ) );
	m.Add( 333, ValueWithUnion( 4.0, "ccc" ) );

	Map_t::iterator	iter;
	ASSERT( m.Find( 222, iter ) );
#endif
}


extern void Test_Containers_HashSet ()
{
	HashSet_Test1();
	HashSet_Test2();
	HashSet_Test3();

	MultiHashSet_Test1();

	HashMap_Test1();
	HashMap_Test2();
	HashMap_Test3();
}
