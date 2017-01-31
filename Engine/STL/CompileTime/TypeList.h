// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "TypeListUtils.h"

namespace GX_STL
{
namespace CompileTime
{
	

	//
	// Type List
	//

	template <typename Left, typename Right = TypeListEnd>
	struct TypeList : public _ctime_hidden_::TypeListBase
	{
		// left type can't be the typelist!
		STATIC_ASSERT( not IsTypeList< Left > );

	// types
	public:
		typedef TypeList< Left, Right >		Self;

		typedef Left	left_t;
		typedef Right	right_t;

		typedef bool	_is_type_list;


	// static values
	private:
		template <usize Index>
		struct _Get {
			STATIC_ASSERT( Index < _ctime_hidden_::TTypeList_Length< Self >::value );

			typedef typename _ctime_hidden_::TTypeList_GetElem< Self, Index >::type		type;
		};
		
		template <usize Index, bool InRange>	struct _GetNoCheck2					{ typedef typename _Get< Index >::type	type; };
		template <usize Index>					struct _GetNoCheck2< Index, false > { typedef TypeListEnd					type; };

		template <usize Index>
		struct _GetNoCheck {
			typedef typename _GetNoCheck2< Index, (Index < _ctime_hidden_::TTypeList_Length< Self >::value) >::type		type;
		};

		template <typename Type>
		struct _PushBack {
			STATIC_ASSERT( not IsTypeList< Type > );
			typedef typename _ctime_hidden_::TTypeList_PushBack< Self, Type >::type		type;
		};

		template <typename Type>
		struct _PushFront {
			STATIC_ASSERT( not IsTypeList< Type > );
			typedef typename _ctime_hidden_::TTypeList_PushFront< Self, Type >::type	type;
		};


	public:
		//	ForEach and ReverseForEach example:
		//
		//	template <usize Index, typename Type, typename PrevFuncResult, typename Typelist>
		//	struct Func
		//	{
		//		typedef TypeList< Type, PrevFuncResult >	result;
		//	};

		// ForEach
		template <template <usize Index, typename Type, typename PrevFuncResult, typename Typelist> class Func, typename InitType = TypeListEnd>
		using ForEach			= typename _ctime_hidden_::TTypeList_CompileTimeForEach< Self, Self, 0, Func, InitType >::type;

		// ReverseForEach
		template <template <usize Index, typename Type, typename NextFuncResult, typename Typelist> class Func, typename InitType = TypeListEnd>
		using ReverseForEach	= typename _ctime_hidden_::TTypeList_CompileTimeReverseForEach< Self, Self, 0, Func, InitType >::type;

		
		// Length
		static constexpr usize	Length	= _ctime_hidden_::TTypeList_Length< Self >::value;

		// Empty
		static constexpr bool	Empty	= Length == 0;


		// Get
		template <usize Index>
		using Get		= typename _Get< Index >::type;

		// Front
		using Front		= typename _GetNoCheck< 0 >::type;

		// Back
		using Back		= typename _GetNoCheck< Length-1 >::type;

		// PushBack
		template <typename Type>
		using PushBack	= typename _PushBack< Type >::type;

		// PushFront
		template <typename Type>
		using PushFront	= typename _PushFront< Type >::type;

		// SubList
		template <usize First, usize Count>
		using SubList	= typename _ctime_hidden_::TTypeList_SubList< Self, First, Count >::type;

		// PopBack
		using PopBack	= typename SubList< 0, Length-1 >;

		// PopFront
		using PopFront	= typename SubList< 1, Length-1 >;

		// Append
		template <typename Typelist>
		using Append	= typename _ctime_hidden_::TTypeList_Append< Self, Typelist >::type;


		// FirstIndexOf
		template <typename Type>
		static constexpr usize	FirstIndexOf	= ForEach< _ctime_hidden_::TTypeList_GetFirstIndexOfType< Type >::Func,
															ValueToType< usize, usize(-1) > >::value;
		// IndexOf
		template <typename Type>
		static constexpr usize	IndexOf			= FirstIndexOf< Type >;
		
		// LastIndexOf
		template <typename Type>
		static constexpr usize	LastIndexOf		= ForEach< _ctime_hidden_::TTypeList_GetLastIndexOfType< Type >::Func,
															ValueToType< usize, usize(-1) > >::value;
		// HasType
		template <typename Type>
		static constexpr bool	HasType			= ( IndexOf< Type > != usize(-1) );

		// Equal
		template <typename TTypeList>
		static constexpr bool	Equal			= (Length == TTypeList::Length) and
													ForEach< _ctime_hidden_::TTypeList_Equal< TTypeList >::Func, ValueToType<bool, true> >::value;
		// IsType
		template <usize Index, typename Type>
		static constexpr bool	IsType			= IsSameTypes< typename Get< Index >, Type >;


		// MaxSizeOf
		static constexpr usize	MaxSizeOf () 	{ return ForEach< _ctime_hidden_::TTypeList_MaxSize, ValueToType<usize, 0> >::value; }

		// MaxAlignOf
		static constexpr usize	MaxAlignOf ()	{ return ForEach< _ctime_hidden_::TTypeList_MaxAlign, ValueToType<usize, 0> >::value; }

		// IsAllSameAs
		template <typename Type>
		static constexpr bool	AllAreSameAs	= ForEach< _ctime_hidden_::TTypeList_IsAllSameAs<Type>::Func,
															ValueToType< bool, true > >::value;
		// IsAllSame
		static constexpr bool	AllAreSame		= AllAreSameAs< Front >;


	// methods
	public:

		// RuntimeForEach example:
		//
		//	struct Func
		//	{
		//		usize result;
		//
		//		template <typename T, usize Index>
		//		void Process ()	{ result += sizeof(T); }
		//	};
		
		// RuntimeForEach
		template <typename Func>
		static void RuntimeForEach (Func &func)
		{
			_ctime_hidden_::TTypeList_ForEach< Self, Func, 0 >::Iterate( func );
		}
	};
	


	template <typename Left, typename Right>
	template <typename Type>
	constexpr usize	TypeList< Left, Right >::FirstIndexOf;
		
	template <typename Left, typename Right>
	template <typename Type>
	constexpr usize	TypeList< Left, Right >::IndexOf;
		
	template <typename Left, typename Right>
	template <typename Type>
	constexpr usize	TypeList< Left, Right >::LastIndexOf;
	
	template <typename Left, typename Right>
	template <typename Type>
	constexpr bool	TypeList< Left, Right >::HasType;
	
	template <typename Left, typename Right>
	template <typename TTypeList>
	constexpr bool	TypeList< Left, Right >::Equal;
	
	template <typename Left, typename Right>
	template <usize Index, typename Type>
	constexpr bool	TypeList< Left, Right >::IsType;


}	// CompileTime
}	// GX_STL
