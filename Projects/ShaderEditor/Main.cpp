// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Core/ShaderEditor.h"

#include "Samples/RayTracing/RayTracingSamples.h"
#include "Samples/SimpleLandscape/SimpleLandscapeSample.h"
#include "Samples/TessLandscape/TessLandscapeSample.h"
#include "Samples/Particles/ParticlesSample.h"
#include "Samples/InteractiveParticles/InteractiveParticlesSample.h"


namespace ShaderEditor
{

	extern void InitializeShaderSamples (Ptr< ShaderEditorCore > core)
	{
		core->AddSample( New<InteractiveParticlesSample>( core->SubSystems() ) );
		core->AddSample( New<SimpleLandscapeSample>( core->SubSystems() ) );
		core->AddSample( New<ParticlesSample>( core->SubSystems() ) );
		core->AddSample( New<RayTracingSamples>( core->SubSystems() ) );
		//core->AddSample( New<TessLandscapeSample>( core->SubSystems() ) );
	}

}	// ShaderEditor
