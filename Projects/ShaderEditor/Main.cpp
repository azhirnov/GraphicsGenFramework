// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Core/ShaderEditor.h"

#include "Samples/RayTracing/RayTracingSamples.h"
#include "Samples/SimpleLandscape/SimpleLandscapeSample.h"
#include "Samples/TessLandscape/TessLandscapeSample.h"
#include "Samples/Particles/ParticlesSample.h"


namespace ShaderEditor
{

	extern void InitializeShaderSamples (Ptr< ShaderEditorCore > core)
	{
		core->AddSample( SimpleLandscapeSample::New( core->SubSystems() ) );
		//core->AddSample( ParticlesSample::New( core->SubSystems() ) );
		core->AddSample( RayTracingSamples::New( core->SubSystems() ) );
		//core->AddSample( TessLandscapeSample::New( core->SubSystems() ) );
	}

}	// ShaderEditor
