// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "GraphicsQuery.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	GraphicsQuery::GraphicsQuery (const SubSystemsRef ss) :
		BaseObject( ss ),
		_target( EQuery::Unknown ),		_index( -1 ),
		_resultAvailable( false ),		_result( 0 ),
		_resultCached( false )
	{
	}

/*
=================================================
	destructor
=================================================
*/
	GraphicsQuery::~GraphicsQuery ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GraphicsQuery::Create ()
	{
		_Delete();

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateQuery( OUT _id ) );
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void GraphicsQuery::_Delete ()
	{
		if ( SubSystems()->Get< GraphicsEngine >() )
		{
			SubSystems()->Get< GraphicsEngine >()->GetContext()->DeleteQuery( _id );
		}

		_target				= EQuery::Unknown;
		_index				= uint(-1);
		_result				= 0;
		_resultAvailable	= false;
		_resultCached		= false;
	}
	
/*
=================================================
	Begin
=================================================
*/
	void GraphicsQuery::Begin (EQuery::type target, uint index)
	{
		End();

		_target				= target;
		_index				= index;
		_resultAvailable	= false;
		_resultCached		= false;
		_result				= 0;

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->BeginQuery( _id, _target, _index );
	}
	
/*
=================================================
	End
=================================================
*/
	void GraphicsQuery::End ()
	{
		_resultAvailable = false;
		_resultCached	 = false;

		if ( _index != uint(-1) )
		{
			SubSystems()->Get< GraphicsEngine >()->GetStateManager()->EndQuery( _id, _target, _index );

			_target	= EQuery::Unknown;
			_index	= uint(-1);
		}
	}
	
/*
=================================================
	IsResultAvailable
=================================================
*/
	bool GraphicsQuery::IsResultAvailable ()
	{
		if ( _resultAvailable )
			return true;

		_resultAvailable = _IsResultAvailable();
		_resultCached	 = false;

		return _resultAvailable;
	}
	
/*
=================================================
	GetResult
=================================================
*/
	ulong GraphicsQuery::GetResult ()
	{
		if ( _resultCached )
			return _result;

		_resultCached	= true;
		_result			= _SyncResult();

		return _result;
	}


}	// Graphics
}	// Engine
