// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#include "Core/ShaderEditor.h"

#include "Samples/Shadertoy/ShadertoySamples.h"
#include "Samples/TileLandscape/TileLandscapeSample.h"

namespace ShaderEditor
{

	extern void InitializeShaderSamples (Ptr< ShaderEditorCore > core)
	{
		core->AddSample( TileLandscapeSample::New( core->SubSystems() ) );
		core->AddSample( ShadertoySamples::New( core->SubSystems() ) );
	}

}	// ShaderEditor
