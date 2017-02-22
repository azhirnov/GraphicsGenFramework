// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Wrapper for (member) function pointer.
*/

#pragma once

#include "Engine/STL/CompileTime/TypeTraits.h"
#include "Engine/STL/Memory/MemFunc.h"
#include "Engine/STL/Memory/PlacementNew.h"
#include "Engine/STL/Dimensions/ByteAndBit.h"
#include "Function.h"

namespace GX_STL
{
namespace GXTypes
{

	template <typename Func, usize BufSize = GlobalConst::STL_DelegateBufferSize>
	struct Delegate;
	
	template <typename Func, usize BufSize = GlobalConst::STL_DelegateBufferSize>
	struct Event {};


	namespace _types_hidden_
	{
		template <typename Func>
		struct DelegateInterface;


		//
		// Delegate interface
		//
		template <typename Ret, typename ...Args>
		struct DelegateInterface< Ret (Args...) >
		{
		// types
			typedef DelegateInterface< Ret (Args...) >	Interface_t;

		// methods
			virtual ~DelegateInterface ()				{}
			virtual bool		IsValid ()				const = 0;
			virtual Ret			Call (Args...)			const = 0;
			//virtual void		Swap (Interface_t *)	= 0;
			virtual void		MoveTo (BinaryBuffer)	= 0;
			virtual void		CopyTo (BinaryBuffer)	const = 0;
			virtual TypeId		TypeIdOf ()				const = 0;
			virtual BytesU		Size ()					const = 0;
		
			bool Cmp (const Interface_t *right) const
			{
				const BytesU size = Size();
				return	( size == right->Size() ) and
						( TypeIdOf() == right->TypeIdOf() ) and
						UnsafeMem::MemCmp( (void *)this, (void *)right, size ) == 0;
			}
		
			bool Less (const Interface_t *right) const
			{
				const BytesU	size0 = Size();
				const BytesU	size1 = right->Size();
				const TypeId	type0 = TypeIdOf();
				const TypeId	type1 = right->TypeIdOf();
			
				if ( type0 == type1 ) {
					if ( size0 == size1 )
						return UnsafeMem::MemCmp( (void *)this, (void *)right, size0 ) < 0;
					else
						return size0 < size1;
				}
				return type0 < type1;
			}
		
			bool Greater (const Interface_t *right) const
			{
				const BytesU	size0 = Size();
				const BytesU	size1 = right->Size();
				const TypeId	type0 = TypeIdOf();
				const TypeId	type1 = right->TypeIdOf();
			
				if ( type0 == type1 ) {
					if ( size0 == size1 )
						return UnsafeMem::MemCmp( (void *)this, (void *)right, size0 ) > 0;
					else
						return size0 > size1;
				}
				return type0 > type1;
			}
		};


		//
		// Static Delegate
		//
		template <typename Ret, typename ...Args>
		struct StaticDelegate final : DelegateInterface< Ret (Args...) >
		{
		// types
		public:
			typedef StaticDelegate< Ret, Args... >		Self;
			typedef Ret (* Function_t) (Args...);

		// variables
		private:
			Function_t	_func;

		// methods
		public:
			explicit StaticDelegate (Function_t fn) noexcept : _func(fn) {}

			bool		IsValid ()				const override	{ return _func != null; }
			Ret			Call (Args... args)		const override	{ return _func( FW<Args>(args)... ); }
			TypeId		TypeIdOf ()				const override	{ return GXTypes::TypeIdOf( *this ); }
			BytesU		Size ()					const override	{ return SizeOf( *this ); }
			
			void MoveTo (BinaryBuffer buf) override
			{
				PlacementNew< Self >( buf, RVREF( *this ) );
			}

			void CopyTo (BinaryBuffer buf) const override
			{
				PlacementNew< Self >( buf, *this );
			}
		};
		

		//
		// Member Delegate
		//
		template <typename C, typename Class, typename Ret, typename ...Args>
		struct MemberDelegate final : DelegateInterface< Ret (Args...) >
		{
		// types
		public:
			typedef MemberDelegate< C, Class, Ret, Args... >	Self;
			typedef C											Ptr_t;
			typedef Ret (Class:: *Function_t) (Args...);

		// variables
		private:
			Ptr_t			_classPtr;
			Function_t		_func;

		// methods
		public:
			MemberDelegate (const Ptr_t &ptr, Function_t fn) noexcept : _classPtr(ptr), _func(fn) {}
			MemberDelegate (Ptr_t &&ptr, Function_t fn) noexcept : _classPtr( RVREF(ptr) ), _func(fn) {}

			bool		IsValid ()			const override	{ return _func != null and _classPtr != null; }
			Ret			Call (Args... args)	const override	{ return ((*_classPtr).*_func)( FW<Args>(args)... ); }
			TypeId		TypeIdOf ()			const override	{ return GXTypes::TypeIdOf( *this ); }
			BytesU		Size ()				const override	{ return SizeOf( *this ); }
			
			void MoveTo (BinaryBuffer buf) override
			{
				PlacementNew< Self >( buf, RVREF( *this ) );
			}

			void CopyTo (BinaryBuffer buf) const override
			{
				PlacementNew< Self >( buf, *this );
			}
		};
		

		//
		// Member Delegate (const)
		//
		template <typename C, typename Class, typename Ret, typename ...Args>
		struct MemberDelegateConst final : DelegateInterface< Ret (Args...) >
		{
		// types
		public:
			typedef MemberDelegateConst< C, Class, Ret, Args... >	Self;
			typedef C												Ptr_t;
			typedef Ret (Class:: *Function_t) (Args...) const;
			
		// variables
		private:
			Ptr_t			_classPtr;
			Function_t		_func;

		// methods
		public:
			MemberDelegateConst (const Ptr_t &ptr, Function_t fn) noexcept : _classPtr(ptr), _func(fn) {}
			MemberDelegateConst (Ptr_t &&ptr, Function_t fn) noexcept : _classPtr( RVREF(ptr) ), _func(fn) {}

			bool		IsValid ()			const override	{ return _func != null and _classPtr != null; }
			Ret			Call (Args... args)	const override	{ return ((*_classPtr).*_func)( FW<Args>(args)... ); }
			TypeId		TypeIdOf ()			const override	{ return GXTypes::TypeIdOf( *this ); }
			BytesU		Size ()				const override	{ return SizeOf( *this ); }
			
			void MoveTo (BinaryBuffer buf) override
			{
				PlacementNew< Self >( buf, RVREF( *this ) );
			}

			void CopyTo (BinaryBuffer buf) const override
			{
				PlacementNew< Self >( buf, *this );
			}
		};

	}	// _types_hidden_


	
	//
	// Delegate
	//

	template <typename Ret, typename ...Args, usize BufSize>
	struct alignas(ulong) Delegate< Ret (Args...), BufSize >
	{
	// types
	public:
		typedef Delegate< Ret (Args...), BufSize >	Self;
		typedef Ret									Result_t;

	private:
		typedef _types_hidden_::DelegateInterface< Ret (Args...) >	_Interface_t;
		
		union _Storage_t {
			ulong	maxAlign;
			ubyte	buf[ BufSize ];
		};


	// variables
	private:
		_Storage_t	_storage;

		DEBUG_ONLY(
			_Interface_t*	_interface;
		)


	// methods
	public:
		Delegate (GX_DEFCTOR) noexcept
		{
			DEBUG_ONLY( _interface = _Internal(); )

			_Clear();
		}

		Delegate (const Self &other) noexcept
		{
			DEBUG_ONLY( _interface = _Internal(); )

			_Copy( other );
		}

		Delegate (Self &&other) noexcept
		{
			DEBUG_ONLY( _interface = _Internal(); )

			_Move( RVREF( other ) );
		}

		~Delegate ()
		{
			_Delete();
		}

		forceinline Self& operator = (const Self &other)
		{
			_DeleteNoClear();
			_Copy( other );
			return *this;
		}

		forceinline Self& operator = (Self &&other)
		{
			_DeleteNoClear();
			_Move( RVREF( other ) );
			return *this;
		}

		forceinline operator	bool ()								const	{ return IsValid(); }

		forceinline bool		IsValid ()							const	{ return _IsCreated() DEBUG_ONLY( and _Internal()->IsValid() ); }

		forceinline Result_t	operator () (Args... args)			const	{ ASSERT( IsValid() );  return _Internal()->Call( FW<Args>(args)... ); }
		
		forceinline Result_t	Call (Args... args)					const	{ ASSERT( IsValid() );	return _Internal()->Call( FW<Args>(args)... ); }
		forceinline Result_t	SafeCall (Args... args)				const	{ return IsValid() ? _Internal()->Call( FW<Args>(args)... ) : Result_t(); }

		forceinline TypeId		GetType ()							const	{ return IsValid() ? _Internal()->TypeIdOf() : TypeId(); }

		forceinline bool operator == (const Self &right)			const	{ ASSERT( IsValid() );  return _Internal()->Cmp( right._Internal() );  }
		forceinline bool operator <  (const Self &right)			const	{ ASSERT( IsValid() );  return _Internal()->Less( right._Internal() ); }
		forceinline bool operator >  (const Self &right)			const	{ ASSERT( IsValid() );  return _Internal()->Greater( right._Internal() ); }
		forceinline bool operator != (const Self &right)			const	{ return not (*this == right); }
		forceinline bool operator <= (const Self &right)			const	{ return not (*this >  right); }
		forceinline bool operator >= (const Self &right)			const	{ return not (*this <  right); }

	private:
		forceinline _Interface_t const *	_Internal ()			const	{ return (_Interface_t const*) _storage.buf; }
		forceinline _Interface_t *			_Internal ()					{ return (_Interface_t *) _storage.buf; }
		forceinline BinaryBuffer			_Data ()						{ return BinaryBuffer( _storage.buf ); }
		forceinline bool					_IsCreated ()			const	{ return _storage.maxAlign != 0; }

		forceinline void _Delete ()
		{
			if ( _IsCreated() )
			{
				_Internal()->~_Interface_t();
				_Clear();
			}
		}

		forceinline void _DeleteNoClear ()
		{
			if ( _IsCreated() )
			{
				_Internal()->~_Interface_t();
			}
		}

		forceinline void _Move (Self &&other)
		{
			if ( other._IsCreated() )
			{
				other._Internal()->MoveTo( _Data() );
				other._Clear();
			}
			else
				_Clear();
		}

		forceinline void _Copy (const Self &other)
		{
			if ( other._IsCreated() )
				other._Internal()->CopyTo( _Data() );
			else
				_Clear();
		}

		forceinline void _Clear ()
		{
			_storage.maxAlign = 0;
		}


	// statics
	public:
		forceinline static Self  Create (Ret (*fn) (Args...)) noexcept
		{
			typedef _types_hidden_::StaticDelegate< Ret, Args... >	DI;

			Self	del;
			PlacementNew<DI>( del._Data(), DI( fn ) );
			return del;
		}

		template <typename C, typename Class>
		forceinline static Self  Create (const C &classPtr, Ret (Class:: *fn) (Args...)) noexcept
		{
			STATIC_ASSERT(( _types_hidden_::FB_IsSame< C, Class > ));

			typedef _types_hidden_::MemberDelegate< C, Class, Ret, Args... >	DI;

			Self	del;
			PlacementNew<DI>( del._Data(), DI( classPtr, fn ) );
			return del;
		}

		template <typename C, typename Class>
		forceinline static Self  Create (const C &classPtr, Ret (Class:: *fn) (Args...) const) noexcept
		{
			STATIC_ASSERT(( _types_hidden_::FB_IsSame< C, Class > ));

			typedef _types_hidden_::MemberDelegateConst< C, Class, Ret, Args... >	DI;

			Self	del;
			PlacementNew<DI>( del._Data(), DI( classPtr, fn ) );
			return del;
		}
	};


/*
=================================================
	DelegateBuilder
=================================================
*/
	template <typename Ret, typename ...Args>
	forceinline Delegate< Ret (Args...) >  DelegateBuilder (Ret (*fn) (Args...)) noexcept
	{
		return Delegate< Ret (Args...) >::Create( fn );
	}

	template <typename C, typename Class, typename Ret, typename ...Args>
	forceinline Delegate< Ret (Args...) >  DelegateBuilder (const C &classPtr, Ret (Class:: *fn) (Args...)) noexcept
	{
		return Delegate< Ret (Args...) >::Create( classPtr, fn );
	}

	template <typename C, typename Class, typename Ret, typename ...Args>
	forceinline Delegate< Ret (Args...) >  DelegateBuilder (const C &classPtr, Ret (Class:: *fn) (Args...) const) noexcept
	{
		return Delegate< Ret (Args...) >::Create( classPtr, fn );
	}



	//
	// Event
	//
	
	template <typename Ret, typename ...Args, usize BufSize>
	struct Event< Ret (Args...), BufSize > : public CompileTime::FastCopyable
	{
	// types
	public:
		typedef Ret (* Function_t) (Args...);
		typedef Delegate< Ret (Args...), BufSize >	Delegate_t;
		typedef Event< Ret (Args...), BufSize >		Self;
		typedef Set< Delegate_t >					DelegateSet_t;


	// variables
	private:
		DelegateSet_t	_delegates;


	// methods
	public:
		Event (GX_DEFCTOR) noexcept {}

		Event (Self &&other) noexcept = default;
		Event (const Self &other) noexcept = default;

		Self& operator = (Self &&) = default;
		Self& operator = (const Self &) = default;


		forceinline void Add (const Delegate_t &del)								{ _delegates.Add( del ); }
		forceinline void Add (Ret (*fn) (Args...))									{ Add( Delegate_t::Create( fn ) ); }

		template <typename C, typename Class>
		forceinline void Add (const C &ptr, Ret (Class:: *fn) (Args...))			{ Add( Delegate_t::Create( ptr, fn ) ); }
		
		template <typename C, typename Class>
		forceinline void Add (const C &ptr, Ret (Class:: *fn) (Args...) const)		{ Add( Delegate_t::Create( ptr, fn ) ); }

		forceinline void Remove (const Delegate_t &del)								{ _delegates.Erase( del ); }
		forceinline void Remove (Ret (*fn) (Args...))								{ Remove( Delegate_t::Create( fn ) ); }

		template <typename C, typename Class>
		forceinline void Remove (const C &ptr, Ret (Class:: *fn) (Args...))			{ Remove( Delegate_t::Create( ptr, fn ) ); }
		
		template <typename C, typename Class>
		forceinline void Remove (const C &ptr, Ret (Class:: *fn) (Args...) const)	{ Remove( Delegate_t::Create( ptr, fn ) ); }

		forceinline void Reserve (usize count)										{ _delegates.Reserve( count ); }
		forceinline void Clear ()													{ _delegates.Clear(); }


		forceinline void Call (Args... args)					const		{ FOR( i, _delegates ) { _delegates[i].Call( FW<Args>(args)... ); } }

		forceinline void operator () (Args... args)				const		{ FOR( i, _delegates ) { _delegates[i].Call( FW<Args>(args)... ); } }

		forceinline bool Empty ()								const		{ return _delegates.Empty(); }

		forceinline usize Count ()								const		{ return _delegates.Count(); }

		forceinline Delegate_t const&	Get (usize index)		const		{ return _delegates[i]; }


		forceinline friend void SwapValues (Self &left, Self &right)
		{
			SwapValues( left._delegates, right._delegates );
		}
	};

	
	template <typename Ret, typename ...Args, usize BufSize>
	struct Event< Delegate< Ret (Args...), BufSize > > : Event< Ret (Args...), BufSize >
	{};


}	// GXTypes
}	// GX_STL
