// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "VertexDesc.h"

namespace Engine
{
namespace Graphics
{
	
	//
	// Vertex Attribs
	//

	class VertexAttribsDesc : public CompileTime::PODStruct		// TODO: copy from StaticArray
	{
		friend struct VertexAttribsState;

	// types
	public:
		enum AttribIndex {};
		enum BufferBindingIndex {};

		typedef VertexAttribsDesc		Self;
		
	private:
		//
		// Attrib
		//
		struct Attrib : public CompileTime::PODStruct
		{
		// variables
			EAttribute::type	type;
			AttribIndex			index;
			BytesU				offset;
			BufferBindingIndex	bufferIndex;
			bool				enabled;

		// methods
			Attrib (GX_DEFCTOR) :
				type( EAttribute::Unknown ),
				index( AttribIndex(-1) ),
				offset( 0 ),
				bufferIndex( BufferBindingIndex(0) ),
				enabled( false )
			{}

			Attrib (AttribIndex index, EAttribute::type type, BytesU offset, BufferBindingIndex bufferIndex = BufferBindingIndex(0)) :
				type( type ),
				index( index ),
				offset( offset ),
				bufferIndex( bufferIndex ),
				enabled( true )
			{}

			template <typename ClassType, typename ValueType>
			Attrib (AttribIndex index, ValueType ClassType:: *vertex, bool norm, BufferBindingIndex bufferIndex = BufferBindingIndex(0)) :
				type( EAttribute::SetNormalized( VertexDesc< ValueType >::attrib, norm ) ),
				index( index ),
				offset( OffsetOf( vertex ) ),
				bufferIndex( bufferIndex ),
				enabled( true )
			{}

			bool IsEnabled () const
			{
				return index != AttribIndex(-1) and enabled;
			}

			EAttribute::type ToVSInputType () const
			{
				return EAttribute::ValueType::ToVSInputType( type );
			}

			bool operator == (const Attrib &right) const
			{
				return	enabled		== right.enabled	and (
						enabled ? (
						type		== right.type		and
						index		== right.index		and
						offset		== right.offset		and
						bufferIndex	== right.bufferIndex
						) : true );
			}

			bool operator != (const Attrib &right) const
			{
				return not (*this == right);
			}
		};

		typedef StaticArray< Attrib, GlobalConst::Graphics_MaxAttribs >	attribs_t;


	// variables
	private:
		attribs_t		_attribs;


	// methods
	public:
		VertexAttribsDesc (GX_DEFCTOR)
		{}
		

		Self & Add (AttribIndex index, EAttribute::type type, BytesU offset, BufferBindingIndex bufferIndex = BufferBindingIndex(0))
		{
			// attrib allready defined
			ASSERT( _attribs[index].index == AttribIndex(-1) );

			_attribs[index] = Attrib( index, type, offset, bufferIndex );
			return *this;
		}

		
		template <typename ClassType, typename ValueType>
		Self & Add (AttribIndex index, ValueType ClassType:: *vertex, bool norm = false, BufferBindingIndex bufferIndex = BufferBindingIndex(0))
		{
			// attrib allready defined
			ASSERT( _attribs[index].index == AttribIndex(-1) );

			_attribs[index] = Attrib( index, vertex, norm, bufferIndex );
			return *this;
		}


		void Clear ()
		{
			_attribs.Clear();
		}


		usize Count () const
		{
			return _attribs.Count();
		}


		Attrib const & operator [] (usize i) const
		{
			return _attribs[i];
		}


		bool operator == (const VertexAttribsDesc &right) const
		{
			FOR( i, _attribs )
			{
				if ( _attribs[i] != right._attribs[i] )
					return false;
			}
			return true;
		}
	};



	//
	// Vertex Attributes State
	//

	struct VertexAttribsState : public CompileTime::PODStruct		// TODO: copy from StaticArray
	{
	// types
	public:
		typedef VertexAttribsDesc::AttribIndex	AttribIndex;
		typedef VertexAttribsState				Self;

	private:
		//
		// Attrib
		//
		struct Attrib : public CompileTime::PODStruct
		{
		// variables
			EAttribute::type		type;
			AttribIndex				index;
			bool					enabled;

		// methods
			Attrib () :
				type( EAttribute::Unknown ),
				index( AttribIndex(-1) ),
				enabled( false )
			{}

			Attrib (EAttribute::type type, AttribIndex index) :
				type( type ),
				index( index ),
				enabled( true )
			{}

			bool operator == (const Attrib &right) const
			{
				return	enabled	== right.enabled	and (
						enabled ? (
						type	== right.type		and
						index	== right.index ) :
						true );
			}

			bool operator != (const Attrib &right) const
			{
				return not (*this == right);
			}

			DEBUG_ONLY(
			String ToString () const
			{
				EValueTypeInfo::type	val_type = EValueTypeInfo::type( type & EValueTypeInfo::_TYPE_MASK );
				uint					count	 = (type & EValueTypeInfo::_COL_MASK) >> EValueTypeInfo::_COL_OFF;
				String					str( "{ type: " );

				switch ( val_type )
				{
					case EValueTypeInfo::_INT		:	str << "int";		break;
					case EValueTypeInfo::_UINT		:	str << "uint";		break;
					case EValueTypeInfo::_FLOAT		:	str << "float";		break;
					case EValueTypeInfo::_DOUBLE	:	str << "double";	break;
					default							:	str << "<>";		break;
				}
				str << count << "; index: " << index << " }";
				return str;
			})
		};

		typedef StaticArray< Attrib, GlobalConst::Graphics_MaxAttribs >	attribs_t;


	// variables
	private:
		attribs_t	_attribs;


	// methods
	public:
		VertexAttribsState (GX_DEFCTOR)
		{}


		explicit
		VertexAttribsState (const VertexAttribsDesc &desc)
		{
			STATIC_ASSERT( VertexAttribsDesc::attribs_t::STATIC_COUNT == attribs_t::STATIC_COUNT );

			FOR( i, desc._attribs )
			{
				VertexAttribsDesc::Attrib const &	attr = desc._attribs[i];

				if ( desc._attribs[i].enabled )
					_attribs[i] = Attrib( attr.ToVSInputType(), attr.index );
				else
					_attribs[i] = Attrib();
			}
		}
		

		Self & Add (AttribIndex index, EAttribute::type type)
		{
			// attrib allready defined
			ASSERT( _attribs[index].index == AttribIndex(-1) );

			_attribs[index] = Attrib( EAttribute::ValueType::ToVSInputType( type ), index );
			return *this;
		}


		usize Count () const
		{
			return _attribs.Count();
		}


		Attrib const & operator [] (usize index) const
		{
			return _attribs[ index ];
		}


		bool operator == (const VertexAttribsState &right) const
		{
			FOR( i, _attribs )
			{
				if ( _attribs[i] != right._attribs[i] )
					return false;
			}
			return true;
		}


		bool operator != (const VertexAttribsState &right) const
		{
			return not (*this == right);
		}


		DEBUG_ONLY(
		String ToString () const
		{
			String	str;

			FOR( i, _attribs )
			{
				if ( _attribs[i].enabled )
					str << _attribs[i].ToString() << "\n";
			}
			return str;
		})
	};


}	// Graphics
}	// Engine
