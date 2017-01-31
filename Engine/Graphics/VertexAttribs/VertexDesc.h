// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Types/Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Vertex Info
	//

	template <typename T>
	struct VertexDesc
	{
	};


	// Float types
	template <>
	struct VertexDesc <half>
	{
		typedef half					type;
		static const EAttribute::type	attrib = EAttribute::Half;
	};
	template <>

	struct VertexDesc <float>
	{
		typedef float					type;
		static const EAttribute::type	attrib = EAttribute::Float;
	};

	template <>
	struct VertexDesc <double>
	{
		typedef double					type;
		static const EAttribute::type	attrib = EAttribute::Double;
	};


	// Integer types
	template <>
	struct VertexDesc <byte>
	{
		typedef byte					type;
		static const EAttribute::type	attrib = EAttribute::Byte;
	};

	template <>
	struct VertexDesc <ubyte>
	{
		typedef ubyte					type;
		static const EAttribute::type	attrib = EAttribute::UByte;
	};


	template <>
	struct VertexDesc <short>
	{
		typedef short					type;
		static const EAttribute::type	attrib = EAttribute::Short;
	};

	template <>
	struct VertexDesc <ushort>
	{
		typedef ushort					type;
		static const EAttribute::type	attrib = EAttribute::UShort;
	};


	template <>
	struct VertexDesc <int>
	{
		typedef short					type;
		static const EAttribute::type	attrib = EAttribute::Int;
	};

	template <>
	struct VertexDesc <uint>
	{
		typedef ushort					type;
		static const EAttribute::type	attrib = EAttribute::UInt;
	};


	// Vector types
	template <typename T, usize I>
	struct VertexDesc < Vec<T,I> >
	{
		typedef Vec<T,I>				type;
		static const EAttribute::type	attrib = EAttribute::type( (VertexDesc< typename type::value_t >::attrib & EValueTypeInfo::_TYPE_FLAG_MASK) | (I << EValueTypeInfo::_COL_OFF) );
	};


	template <>
	struct VertexDesc <color4u>
	{
		typedef color4u					type;
		static const EAttribute::type	attrib = EAttribute::UByte4_Norm;
	};
	

}	// Graphics
}	// Engine
