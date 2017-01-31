// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ParticleRenderer.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	ParticleRenderer::ParticleRenderer (const SubSystemsRef ss) :
		BaseObject( ss )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	ParticleRenderer::~ParticleRenderer ()
	{
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool ParticleRenderer::Initialize ()
	{
		CHECK_ERR( Reload() );
		return true;
	}
	
/*
=================================================
	Reload
=================================================
*/
	bool ParticleRenderer::Reload ()
	{
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void ParticleRenderer::Deinitialize ()
	{
	}

}	// ShaderEditor
