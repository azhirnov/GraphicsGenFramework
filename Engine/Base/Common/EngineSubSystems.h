// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Common.h"

namespace Engine
{
namespace Base
{


	//
	// Const Reference
	//

	template <typename T>
	struct ConstReference
	{
	// types
	public:
		typedef ConstReference<T>	Self;


	// variables
	private:
		Ptr< T >	_value;


	// methods
	private:
		ConstReference () {}
		
		ConstReference (T &&value) {}

		void operator = (const Self &) {}

	public:
		explicit
		ConstReference (T &value) : _value(&value) {}

		ConstReference (const Self &other) : _value(other._value) {}

		explicit
		ConstReference (void *ptr) : _value(null) { ASSERT( ptr == null ); }

		T *			operator -> ()			{ return _value; }
		T *			operator -> ()	const	{ return (T *)_value; }
	};

}	// Base



	//
	// SubSystems TypeList
	//

	template <typename Typelist>
	struct SubSystemsTypeList
	{
	private:
		STATIC_ASSERT( CompileTime::IsTypeList< Typelist > );

		template <typename Type>
		struct _IndexOf {
			static const usize	value = Typelist::IndexOf< Type >;
			STATIC_ASSERT( value != usize(-1) );
		};

		template <typename Typelist2>
		struct _Append {
			STATIC_ASSERT( CompileTime::IsTypeList< Typelist2 > );
			typedef typename Typelist::template Append< Typelist2 >	type;
		};

	public:
		template <typename Typelist2>
		using Append = SubSystemsTypeList< typename _Append< Typelist2 >::type >;
		
		template <typename Type>
		static constexpr usize IndexOf = _IndexOf< Type >::value;
	};
	
	template <typename Typelist>
	template <typename Type>
	constexpr usize SubSystemsTypeList< Typelist >::IndexOf;


	
	// Base
	typedef SubSystemsTypeList<
		CompileTime::TypeListFrom<
			Base::Application,
			Base::Platform,
			Base::ThreadManager,
			Base::ResourceManager,
			Base::FileManager,
			Base::LocaleManager,
			Base::Input,
			Base::SerializableObjectFactory > >	BaseSubSystemsTypeList_t;
	
	// Graphics
	typedef BaseSubSystemsTypeList_t::Append<
		CompileTime::TypeListFrom<
			Graphics::GraphicsEngine,
			Compute::ComputeEngine > >			GraphicsSubSystemsTypeList_t;
	
	// Audio
	typedef GraphicsSubSystemsTypeList_t::Append<
		CompileTime::TypeListFrom<
			Audio::AudioEngine > >				AudioSubSystemsTypeList_t;



	//
	// All Engine subsystems
	//

	class EngineSubSystems : public Noncopyable
	{
	// types
	private:
		template <typename T> struct Setter {};

		struct Item
		{
			void *		ptr = null;
			TypeId		id;

			template <typename T>
			void Set (T *p)
			{
				CompileTime::MustBeSameSize< decltype(ptr), decltype(p) >();

				ASSERT( (ptr == null) == (p != null) );
				ASSERT( TypeIdOf<T>() == id or id == TypeId() );

				ptr = reinterpret_cast< void *>( p );
				id	= TypeIdOf<T>();
			}

			template <typename T>
			Ptr<T> Get ()
			{
				ASSERT( TypeIdOf<T>() == id );
				return reinterpret_cast< T *>( ptr );
			}
		};

		typedef StaticArray< Item, GlobalConst::Base_MaxSubSystems >	ItemsArray_t;


	// variables
	private:
		ItemsArray_t		_items;
		usize				_threadId;


	// methods
	public:
		EngineSubSystems () :
			_threadId( OS::CurrentThread::GetCurrentThreadId() )
		{}

		template <typename T>
		Ptr<T> Get ();

		template <typename T>
		Setter<T> GetSetter ()
		{
			ASSERT( _CheckThread() );
			return Setter<T>( this );
		}

	private:
		bool _CheckThread () const
		{
			return _threadId == OS::CurrentThread::GetCurrentThreadId();
		}
	};


	typedef Base::ConstReference< EngineSubSystems >	SubSystemsRef;


#	define GX_SUBSYSTEM_DECL( _typelist_, _type_ ) \
		namespace Engine \
		{ \
			template <> \
			struct EngineSubSystems::Setter< _type_ > \
			{ \
				friend class _type_; \
				Setter (EngineSubSystems *ss) : _ss(ss) {} \
				\
			private: \
				EngineSubSystems *_ss; \
				\
				void Set (_type_ *ptr) \
				{ \
					_ss->_items[ _typelist_::IndexOf< _type_ > ].Set( ptr ); \
				} \
			}; \
			\
			template <> \
			inline Ptr< _type_ > EngineSubSystems::Get () \
			{ \
				ASSERT( _CheckThread() ); \
				return this->_items[ _typelist_::IndexOf< _type_ > ].Get< _type_ >(); \
			} \
		}

}	// Engine

GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::Application );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::Platform );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::ThreadManager );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::ResourceManager );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::FileManager );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::LocaleManager );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::Input );
GX_SUBSYSTEM_DECL( BaseSubSystemsTypeList_t, Base::SerializableObjectFactory );
