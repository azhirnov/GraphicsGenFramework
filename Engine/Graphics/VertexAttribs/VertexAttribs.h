// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "VertexAttribsDesc.h"
#include "Engine/Graphics/Types/Common.h"

namespace Engine
{
namespace Graphics
{

	//
	// Vertex Attribs
	//

	class VertexAttribs : public Referenced
	{
	// variables
	private:
		VertexAttribsID			_id;
		VertexAttribsDesc		_desc;
		VertexAttribsState		_state;


	// methods
	public:
		VertexAttribs (const VertexAttribsID &id, const VertexAttribsDesc &desc) :
			_id( id ),
			_desc( desc ),
			_state( desc )
		{}

		VertexAttribsID const &		GetID ()			const	{ return _id; }

		VertexAttribsDesc const &	GetDescriptor ()	const	{ return _desc; }

		VertexAttribsState const &	GetAttribsState ()	const	{ return _state; }


		static VertexAttribsPtr  New (const VertexAttribsID &id, const VertexAttribsDesc &desc)
		{
			return new VertexAttribs( id, desc );
		}
	};

	
}	// Graphics
}	// Engine
