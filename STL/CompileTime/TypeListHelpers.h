// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "PlainOldDataType.h"
#include "TypeList.h"

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
		template <bool UseCppStandard>
		struct MainType_Choose
		{
			template <usize Index, typename Type, typename PrevFuncResult, typename Typelist>
			struct Func
			{
				// change software types to hardware
				static const uint	change_to =
					not CompileTime::IsSoftware< Type > ? 0 :
					CompileTime::IsFloat< Type > or CompileTime::IsFixed< Type > ? 1 :
					CompileTime::IsSigned< Type > ? 2 : 3;

				typedef typename CompileTime::ChooseType< change_to,
									Type,
									typename CompileTime::NearFloat::FromType< Type >,
									typename CompileTime::NearInt::FromType< Type >,
									typename CompileTime::NearUInt::FromType< Type > >	left;

				typedef PrevFuncResult	right;

				static const usize	left_size	= CompileTime::SizeOf<left>::bytes;
				static const usize	right_size	= CompileTime::SizeOf<right>::bytes;

				static const uint	choose_type =
					( not UseCppStandard and CompileTime::IsSameTypes< left, right > ) ? 0 :
					// float types > int types
					CompileTime::IsFloat< left > and CompileTime::IsFloat< right > ? ( left_size >= right_size ? 0 : 1 ) :
					CompileTime::IsFloat< left > ? 0 :
					CompileTime::IsFloat< right > ? 1 :
					// small int types converts to 'int'
					( UseCppStandard and CompileTime::Max< usize, left_size, right_size > < sizeof(int) ) ? 2 :
					// biggest int type
					left_size == right_size and (CompileTime::IsUnsigned< left > or CompileTime::IsUnsigned< right >) ? 
						// unsigned int types > signed types
					(CompileTime::IsUnsigned< left > ? 0 : 1) :
					left_size >= right_size ? 0 : 1;

				typedef typename CompileTime::ChooseType< choose_type, left, right, int >	result;
			};
		};
	}	// _ctime_hidden_
	
	
	template <typename ...Types>
	using MainType = typename CompileTime::TypeListFrom< Types... >::template
						ForEach< _ctime_hidden_::MainType_Choose<false>::Func, void >;

	template <typename ...Types>
	using MainTypeStd = typename CompileTime::TypeListFrom< Types... >::template
							ForEach< _ctime_hidden_::MainType_Choose<true>::Func, void >;
	


}	// CompileTime
}	// GX_STL
