// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "PlainOldDataType.h"
#include "TypeList.h"
#include "NearType.h"

namespace GX_STL
{
namespace CompileTime
{
	
	//
	// Type List From types
	//
	
	namespace _ctime_hidden_
	{
		template <typename T, typename ...Types>
		struct _TypeListFrom
		{
			typedef TypeList< T, typename _TypeListFrom< Types... >::type >	type;
		};

		template <typename T>
		struct _TypeListFrom<T>
		{
			typedef TypeList< T >	type;
		};
	}

	template <typename ...Types>
	using TypeListFrom = typename _ctime_hidden_::_TypeListFrom< Types... >::type;



	//
	// Value List From
	//
	
	namespace _ctime_hidden_
	{
		template <typename T, T Value0, T ...Values>
		struct _ValueListFrom
		{
			typedef TypeList< ValueToType< T, Value0 >, typename _ValueListFrom< T, Values... >::type >	type;
		};
	
		template <typename T, T Value0>
		struct _ValueListFrom< T, Value0 >
		{
			typedef TypeList< ValueToType< T, Value0 > >	type;
		};
	}

	template <typename T, T ...Values>
	using ValueListFrom = typename _ctime_hidden_::_ValueListFrom< T, Values... >::type;



	//
	// Copy Qualifiers From TypeList
	//
	
	namespace _ctime_hidden_
	{
		template <typename Typelist>
		struct CopyQualifiersFromTypeList
		{
			template <usize, typename Type, typename PrevFuncResult, typename>
			struct _TypeList_GetMainQualifier
			{
				typedef ValueToType< ETypeQualifier::type,
					(_SwitchStrongerQualifier< _GetTypeQualifier<Type>::value, PrevFuncResult::value >::value) >	result;
			};

			static const ETypeQualifier::type	qual = Typelist::ForEach< _TypeList_GetMainQualifier,
														ValueToType<ETypeQualifier::type, ETypeQualifier::Unknown> >::value;

			typedef TypeQualifier< qual >	type;
		};

	}	// _ctime_hidden_
	

	//
	// Copy Qualifier Type
	//

	template <typename ...Types>
	using CopyQualifiers =  typename _ctime_hidden_::CopyQualifiersFromTypeList< typename TypeListFrom< Types... > >::type;



	//
	// Choose Type
	//
	
	namespace _ctime_hidden_
	{
		template <uint Index, typename ...Types>
		struct _ChooseType
		{
			typedef typename TypeListFrom< Types... >		TypeList_t;

			STATIC_ASSERT( Index < TypeList_t::Length );

			typedef typename TypeList_t::template Get< Index >	type;
		};
	}

	template <uint Index, typename ...Types>
	using ChooseType = typename _ctime_hidden_::_ChooseType< Index, Types... >::type;



	//
	// Min / Max From
	//
	
	namespace _ctime_hidden_
	{
		template <usize Index, typename Type, typename PrevFuncResult, typename Typelist>
		struct MinFrom_Func
		{
			typedef typename Type::type		type;

			typedef ValueToType< type, Min< type, Type::value, PrevFuncResult::value > >		result;
		};
		
		template <usize Index, typename Type, typename PrevFuncResult, typename Typelist>
		struct MaxFrom_Func
		{
			typedef typename Type::type		type;

			typedef ValueToType< type, Max< type, Type::value, PrevFuncResult::value > >		result;
		};

	}	// _ctime_hidden_


	template <typename T, T ...Args>
	constexpr T MinFrom ()
	{
		typedef typename ValueListFrom< T, Args... >	ValueList_t;

		return ValueList_t::ForEach< _ctime_hidden_::MinFrom_Func, ValueList_t::Front >::value;
	}

	template <typename T, T ...Args>
	constexpr T MaxFrom ()
	{
		typedef typename ValueListFrom< T, Args... >	ValueList_t;

		return ValueList_t::ForEach< _ctime_hidden_::MaxFrom_Func, ValueList_t::Front >::value;
	}



	//
	// Main Type
	//
	
	namespace _ctime_hidden_
	{
		template <typename T, typename Other>
		struct _DeferredCreateWith
		{
			typedef typename TypeInfo<T>::template CreateWith<Other>	type;
		};

		template <typename T0, typename T1>
		struct _ToSameCapacity
		{
			static const uint	c0 = TypeInfo<T0>::Count();
			static const uint	c1 = TypeInfo<T1>::Count();
			
			typedef typename TypeInfo<T0>::inner_type				t0;
			typedef typename TypeInfo<T1>::inner_type				t1;

			typedef typename SwitchType< (c0 >= c1 or IsVoid<T0>), T0, typename _DeferredCreateWith<T1,t0>::type >		left;
			typedef typename SwitchType< (c1 >= c0 or IsVoid<T1>), T1, typename _DeferredCreateWith<T0,t1>::type >		right;
		};

		template <uint Index, typename T0, typename T1, typename T2 = int>
		struct _ToSameWrapper
		{
			typedef typename TypeInfo<T0>::inner_type	t0;
			typedef typename TypeInfo<T1>::inner_type	t1;

			typedef typename SwitchType< not IsWrapper<T0> and IsWrapper<T1>, 
							typename _DeferredCreateWith<T1,t0>::type, T0 >		type0;
			
			typedef typename SwitchType< not IsWrapper<T1> and IsWrapper<T0>,
							typename _DeferredCreateWith<T0,t1>::type, T1 >		type1;
			/*
			typedef typename SwitchType< IsWrapper<T0>,
						typename SwitchType< IsWrapper<T1>,
							typename SwitchType< (SizeOf<T0>::bytes >= SizeOf<T1>::bytes),	// T0 and T1 are wrappers
								typename _DeferredCreateWith<T0,T2>::type,					// from T0
								typename _DeferredCreateWith<T1,T2>::type >,				// from T1
							typename _DeferredCreateWith<T0,T2>::type >,				// only T0 is wrapper
						typename SwitchType< IsWrapper<T1>,
							typename _DeferredCreateWith<T1,T2>::type,					// only T1 is wrapper
							T2 > >			type2;										// none of T0 and T1 is wrappers
							*/
			typedef typename ChooseType< Index, type0, type1, T2 >	type;
		};

		
		template <bool UseCppStandard>
		struct MainType_Choose
		{
			template <usize Index, typename Type, typename PrevFuncResult, typename Typelist>
			struct Func
			{
				// change software types to hardware
				static const uint	change_to =
					not IsSoftware< Type > or IsVoid< Type > ? 0 :
					IsFloat< Type > or IsFixed< Type > ? 1 :
					IsSigned< Type > ? 2 : 3;

				typedef typename ChooseType< change_to,
									Type,
									typename NearFloat::FromType< Type >,
									typename NearInt::FromType< Type >,
									typename NearUInt::FromType< Type > >	hw_left;

				// set same capacity
				//*
				typedef _ToSameCapacity< hw_left, PrevFuncResult >	_same_capacity;

				typedef typename _same_capacity::left	left;
				typedef typename _same_capacity::right	right;
				/*/
				typedef hw_left			left;
				typedef PrevFuncResult	right;
				//*/

				// calc type index
				static const usize	left_size	= SizeOf<left>::bytes;
				static const usize	right_size	= SizeOf<right>::bytes;

				static const usize	left_count	= TypeInfo< left >::Count();
				static const usize	right_count	= TypeInfo< right >::Count();

				static const uint	type_index =
					( not UseCppStandard and IsSameTypes< left, right > ) ? 0 :
					// biggest capacity
					left_count != right_count ? (left_count > right_count ? 0 : 1) :
					// float types > int types
					IsFloat< left > and IsFloat< right > ? ( left_size >= right_size ? 0 : 1 ) :
					IsFloat< left > ? 0 :
					IsFloat< right > ? 1 :
					// small int types converts to 'int'
					( UseCppStandard and Max< usize, left_size, right_size > < sizeof(int) ) ? 2 :
					// biggest int type
					left_size == right_size and (IsUnsigned< left > or IsUnsigned< right >) ? 
					// unsigned int types > signed types
					(IsUnsigned< left > ? 0 : 1) :
					left_size >= right_size ? 0 : 1;

				// choose type with (new) wrapper
				typedef typename _ToSameWrapper< type_index, left, right, int >::type	result;
			};
		};
	}	// _ctime_hidden_
	
	
	template <typename ...Types>
	using MainType = typename TypeListFrom< Types... >::template
						ForEach< _ctime_hidden_::MainType_Choose<false>::Func, void >;

	template <typename ...Types>
	using MainTypeStd = typename TypeListFrom< Types... >::template
							ForEach< _ctime_hidden_::MainType_Choose<true>::Func, void >;
	


}	// CompileTime
}	// GX_STL
