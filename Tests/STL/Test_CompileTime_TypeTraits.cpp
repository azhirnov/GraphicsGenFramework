// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/STL/Engine.STL.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::CompileTime;
using namespace GX_STL::TypeTraits;


struct Test
{
	int memberVar = 0;
	
	const int memberConst = 1;

	float MemberFunc1 ()							{ return 0.0f; }
	int MemberFunc2 (int i, float f)				{ return 0; }
	int MemberFuncConst (int i, float f) const		{ return 0; }

	virtual void VirtMemberFunc (double d)			{}
};

typedef int Test::*Test_memberVar;
typedef int Test::*Test_memberConst;
typedef float (Test::*Test_MemberFunc1) ();
typedef int (Test::*Test_MemberFunc2) (int, float);
typedef int (Test::*Test_MemberFuncConst) (int, float) const;
typedef int (Test::*Test_VirtMemberFunc) (double);
typedef decltype(&Test::MemberFunc1) Test_MemberFunc3;
typedef decltype(&Test::MemberFunc2) Test_MemberFunc4;
typedef int (*Test_StaticFunc) (float, double);

enum ETestEnum {};


// EnableIf //
template <typename T>
static int Test1 (T a, CompileTime::EnableIf< (sizeof(T) > 2), int > dummy = 0)
{
	return 1;
}

template <typename T>
static float Test1 (T b, CompileTime::EnableIf< (sizeof(T) <= 2), int > dummy = 0)
{
	return 2.0f;
}


// SwitchType //
template <typename T>
static int Test2 (CompileTime::SwitchType< (sizeof(T) > 2), T, void > a)
{
	return 1;
}

template <typename T>
static float Test2 (CompileTime::SwitchType< (sizeof(T) <= 2), T, void > a)
{
	return 2.0f;
}


extern void Test_CompileTime_TypeTraits ()
{
	// RemoveQualifier //
	STATIC_ASSERT(( IsSameTypes< int, RemoveQualifier< int > > ));
	STATIC_ASSERT(( IsSameTypes< int, RemoveQualifier< int * > > ));
	STATIC_ASSERT(( IsSameTypes< int, RemoveQualifier< int & > > ));
	STATIC_ASSERT(( IsSameTypes< int, RemoveQualifier< const int > > ));
	STATIC_ASSERT(( IsSameTypes< int, RemoveQualifier< volatile int > > ));
	STATIC_ASSERT(( IsSameTypes< int *, RemoveQualifier< int ** > > ));
	STATIC_ASSERT(( not IsSameTypes< int, RemoveQualifier< int ** > > ));
	STATIC_ASSERT(( not IsSameTypes< int, RemoveQualifier< const int * > > ));


	// IsBaseType //
	STATIC_ASSERT( IsBaseType< int > );
	STATIC_ASSERT( IsBaseType< float > );
	STATIC_ASSERT( not IsBaseType< int * > );
	STATIC_ASSERT( not IsBaseType< const int > );
	STATIC_ASSERT( not IsBaseType< int ** const & > );


	// GetBaseType //
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< int > > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< int *> > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< int **> > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< const int > > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< int && > > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< const volatile int > > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< int ** & > > ));
	STATIC_ASSERT(( IsSameTypes< int, GetBaseType< int const * volatile * > > ));


	// IsPointer //
	STATIC_ASSERT( not IsPointer< int > );
	STATIC_ASSERT( not IsPointer< int & > );
	STATIC_ASSERT( not IsPointer< const int > );
	STATIC_ASSERT( IsPointer< int * > );
	STATIC_ASSERT( IsPointer< int const * > );
	STATIC_ASSERT( IsPointer< int * const > );
	STATIC_ASSERT( IsPointer< int * volatile > );
	STATIC_ASSERT( IsPointer< int const * const > );
	STATIC_ASSERT( not IsPointer< Test_memberVar > );
	STATIC_ASSERT( not IsPointer< Test_MemberFunc1 > );
	//STATIC_ASSERT( not IsPointer< Test_StaticFunc > );
	

	// RemovePointer //
	STATIC_ASSERT(( IsSameTypes< RemovePointer< int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemovePointer< int * >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemovePointer< int const * >, const int > ));
	STATIC_ASSERT(( IsSameTypes< RemovePointer< int const * const>, const int > ));
	STATIC_ASSERT(( IsSameTypes< RemovePointer< int * * >, int * > ));


	// IsReference //
	STATIC_ASSERT( not IsReference< int > );
	STATIC_ASSERT( IsReference< const int & > );
	STATIC_ASSERT( IsReference< int & > );
	STATIC_ASSERT( not IsReference< int * > );
	STATIC_ASSERT( not IsReference< int && > );
	

	// RemoveReference //
	STATIC_ASSERT(( IsSameTypes< RemoveReference< int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveReference< int & >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveReference< int && >, int && > ));
	STATIC_ASSERT(( IsSameTypes< RemoveReference< int const & >, const int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveReference< int * & >, int * > ));


	// IsRValueReference //
	STATIC_ASSERT( IsRValueReference< int && > );
	STATIC_ASSERT( not IsRValueReference< int & > );
	STATIC_ASSERT( not IsRValueReference< volatile int * > );
	

	// RemoveRValueReference //
	STATIC_ASSERT(( IsSameTypes< RemoveRValueReference< int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveRValueReference< int & >, int & > ));
	STATIC_ASSERT(( IsSameTypes< RemoveRValueReference< int && >, int > ));


	// IsConst (like std::is_const) //
	STATIC_ASSERT( not IsConst< int > );
	STATIC_ASSERT(     IsConst< const int > );
	STATIC_ASSERT( not IsConst< const int * > );
	STATIC_ASSERT( not IsConst< int const * > );
	STATIC_ASSERT(     IsConst< int * const > );
	STATIC_ASSERT(     IsConst< int const * const > );
	STATIC_ASSERT( not IsConst< int const& > );
	STATIC_ASSERT( not IsConst< int * > );
	STATIC_ASSERT( not IsConst< int & > );
	STATIC_ASSERT( not IsConst< int [] > );
	//STATIC_ASSERT(     IsConst< const int [54] >::value );


	// CopyConst //
	STATIC_ASSERT(( IsSameTypes< CopyConst< int, float >, float > ));
	STATIC_ASSERT(( IsSameTypes< CopyConst< const int, float >, const float > ));
	
	STATIC_ASSERT(( not IsSameTypes< CopyConst< int, float >, float * > ));
	STATIC_ASSERT(( not IsSameTypes< CopyConst< const int, float >, const float * > ));
	
	STATIC_ASSERT(( IsSameTypes< CopyConstToPointer< int *, float * >, float * > ));
	STATIC_ASSERT(( IsSameTypes< CopyConstToPointer< int * const, float * >, float const * > ));

	STATIC_ASSERT(( not IsSameTypes< CopyConst< int *, float >, float * > ));
	STATIC_ASSERT(( not IsSameTypes< CopyConst< int const *, float >, float const * > ));
	
	STATIC_ASSERT(( IsSameTypes< CopyConstToPointer< int, float * >, float * > ));
	STATIC_ASSERT(( IsSameTypes< CopyConstToPointer< const int, float * >, float const * > ));
	
	STATIC_ASSERT(( IsSameTypes< CopyConstToReference< int, float & >, float & > ));
	STATIC_ASSERT(( IsSameTypes< CopyConstToReference< const int, float & >, float const & > ));


	// Remove* //
	STATIC_ASSERT(( IsSameTypes< RemoveConst< int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConst< const int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConst< const int * >, const int * > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConst< const int & >, const int & > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConst< const volatile int >, volatile int > ));
	
	STATIC_ASSERT(( IsSameTypes< RemoveVolatile< int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveVolatile< volatile int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveVolatile< volatile int * >, volatile int * > ));
	STATIC_ASSERT(( IsSameTypes< RemoveVolatile< volatile int & >, volatile int & > ));
	STATIC_ASSERT(( IsSameTypes< RemoveVolatile< const volatile int >, const int > ));
	
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< const int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< const int * >, const int * > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< volatile int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< volatile int * >, volatile int * > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< const volatile int >, int > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< const volatile int * >, const volatile int * > ));
	STATIC_ASSERT(( IsSameTypes< RemoveConstVolatile< const volatile int & >, const volatile int & > ));
	

	// IsMemberVariablePointer //
	STATIC_ASSERT( IsMemberVariablePointer< Test_memberVar > );
	STATIC_ASSERT( IsMemberVariablePointer< Test_memberConst > );
	STATIC_ASSERT( IsMemberVariablePointer< decltype(&Test::memberConst) > );
	STATIC_ASSERT( not IsMemberVariablePointer< Test_MemberFunc1 > );
	STATIC_ASSERT( not IsMemberVariablePointer< Test_MemberFunc2 > );
	STATIC_ASSERT( not IsMemberVariablePointer< Test_MemberFuncConst > );
	STATIC_ASSERT( not IsMemberVariablePointer< Test_VirtMemberFunc > );
	STATIC_ASSERT( not IsMemberVariablePointer< Test_StaticFunc > );
	STATIC_ASSERT( not IsMemberVariablePointer< int * > );
	STATIC_ASSERT( not IsMemberVariablePointer< int * const * > );


	// IsMemberFunctionPointer //
	STATIC_ASSERT( not IsMemberFunctionPointer< Test_memberVar > );
	STATIC_ASSERT( not IsMemberFunctionPointer< Test_memberConst > );
	STATIC_ASSERT( IsMemberFunctionPointer< Test_MemberFunc1 > );
	STATIC_ASSERT( IsMemberFunctionPointer< Test_MemberFunc2 > );
	STATIC_ASSERT( IsMemberFunctionPointer< Test_MemberFunc3 > );
	STATIC_ASSERT( IsMemberFunctionPointer< Test_MemberFunc4 > );
	STATIC_ASSERT( IsMemberFunctionPointer< Test_MemberFuncConst > );
	STATIC_ASSERT( IsMemberFunctionPointer< Test_VirtMemberFunc > );
	STATIC_ASSERT( not IsMemberFunctionPointer< Test_StaticFunc > );
	STATIC_ASSERT( not IsMemberFunctionPointer< int * > );
	STATIC_ASSERT( not IsMemberFunctionPointer< int * const * > );
	

	// IsFunctionPointer //
	STATIC_ASSERT( not IsFunctionPointer< Test_memberVar > );
	STATIC_ASSERT( not IsFunctionPointer< Test_memberConst > );
	STATIC_ASSERT( not IsFunctionPointer< Test_MemberFunc1 > );
	STATIC_ASSERT( not IsFunctionPointer< Test_MemberFunc2 > );
	STATIC_ASSERT( not IsFunctionPointer< Test_MemberFunc3 > );
	STATIC_ASSERT( not IsFunctionPointer< Test_MemberFunc4 > );
	STATIC_ASSERT( not IsFunctionPointer< Test_MemberFuncConst > );
	STATIC_ASSERT( not IsFunctionPointer< Test_VirtMemberFunc > );
	STATIC_ASSERT( IsFunctionPointer< Test_StaticFunc > );
	STATIC_ASSERT( not IsFunctionPointer< int * > );
	STATIC_ASSERT( not IsFunctionPointer< int * const * > );


	// IsEnum //
	STATIC_ASSERT( IsEnum< ETestEnum > );
	STATIC_ASSERT( not IsEnum< int > );


	// IsBaseOf //
	struct A {};
	struct B {};
	struct A1 : A {};
	struct AB : A, B {};
	struct A2 : A1 {};

	STATIC_ASSERT(( not IsBaseOf< A, B > ));
	STATIC_ASSERT(( IsBaseOf< A, A1 > ));
	STATIC_ASSERT(( IsBaseOf< A, AB > ));
	STATIC_ASSERT(( IsBaseOf< B, AB > ));
	STATIC_ASSERT(( IsBaseOf< A1, A2 > ));
	STATIC_ASSERT(( IsBaseOf< A, A2 > ));
	STATIC_ASSERT(( not IsBaseOf< B, A2 > ));


	// ResultOf //
	//STATIC_ASSERT(( IsSameTypes< ResultOf< Test_memberVar >, int > ));
	STATIC_ASSERT(( IsSameTypes< ResultOf< Test_MemberFunc1 >, float > ));
	STATIC_ASSERT(( IsSameTypes< ResultOf< decltype(&Test::MemberFunc2)(Test, int, float) >, int > ));
	STATIC_ASSERT(( IsSameTypes< std::result_of_t< decltype(&Test::MemberFunc2)(Test, int, float) >, int > ));
	

	int	param = 1;

	// EnableIf //
	int	a = Test1( param );

	// SwitchType //
	int	b = Test1( param );
}
