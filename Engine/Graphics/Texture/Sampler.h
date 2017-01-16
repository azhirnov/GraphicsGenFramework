// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Types/Common.h"

namespace Engine
{
namespace Graphics
{

	//
	// Sampler
	//

	class Sampler : public Referenced
	{
	// variables
	private:
		SamplerState	_state;
		SamplerID		_id;


	// methods
	private:
		Sampler (const SamplerID &id, const SamplerState &state) :
		  _id(id), _state(state)
		{}

		~Sampler ()
		{}
		
	public:
		SamplerID const &	GetSamplerID ()	const	{ return _id; }

		SamplerState const&	GetState ()		const	{ return _state; }


		static SamplerPtr  New (const SamplerID &id, const SamplerState &state)
		{
			return new Sampler( id, state );
		}
	};


}	// Graphics
}	// Engine
