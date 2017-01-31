// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Types/Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Fragment Output State (only colors)
	//

	struct FragmentOutputState : public CompileTime::CopyQualifiers< StaticArray<EFragOutput::type,1> >
	{
	// types
	private:
		typedef StaticArray< EFragOutput::type, GlobalConst::Graphics_MaxColorBuffers >		FragOutputs_t;


	// variables
	private:
		FragOutputs_t	_outputs;


	// methods
	public:
		FragmentOutputState (GX_DEFCTOR) : _outputs( EFragOutput::Unknown )
		{
		}


		usize Count () const
		{
			return _outputs.Count();
		}


		EFragOutput::type operator [] (usize index) const
		{
			return _outputs[ index ];
		}

		EFragOutput::type& operator [] (usize index)
		{
			return _outputs[ index ];
		}

		FragmentOutputState& Set (EFragOutput::type type, usize index)
		{
			_outputs[ index ] = type;
			return *this;
		}


		EFragOutput::type operator [] (ERenderTarget::type target) const
		{
			ASSERT( target >= ERenderTarget::Color0 );
			return _outputs[ target - ERenderTarget::Color0 ];
		}

		EFragOutput::type& operator [] (ERenderTarget::type target)
		{
			ASSERT( target >= ERenderTarget::Color0 );
			return _outputs[ target - ERenderTarget::Color0 ];
		}

		FragmentOutputState& Set (EFragOutput::type type, ERenderTarget::type target)
		{
			ASSERT( target >= ERenderTarget::Color0 );
			_outputs[ target - ERenderTarget::Color0 ] = type;
			return *this;
		}


		bool IsEnabled (usize index) const
		{
			return _outputs[ index ] != EFragOutput::Unknown;
		}


		bool operator == (const FragmentOutputState &right) const
		{
			FOR( i, _outputs )
			{
				const bool	left_enabled	= IsEnabled( i );
				const bool	right_enabled	= right.IsEnabled( i );

				if ( not ( left_enabled == right_enabled and ( not left_enabled or _outputs[i] == right[i] ) ) )
					return false;
			}
			return true;
		}


		bool operator != (const FragmentOutputState &right) const
		{
			return not (*this == right);
		}
	};


}	// Graphics
}	// Engine
