// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Compute/Engine.Compute.h"
#include "../../Shared/Controllers/ScaleBiasController.h"
#include "../../Shared/Controllers/FreeCameraController.h"
#include "../../Shared/Controllers/FlightCameraController.h"
#include "../../Shared/Loaders/ImageLoader.h"

#ifndef COMPUTE_API_OPENGL
#	error Use OpenGL!
#endif

namespace ShaderEditor
{
	using namespace Engine;
	using namespace Engine::Base;
	using namespace Engine::Graphics;
	using namespace Engine::Compute;
	
	class ShaderEditorCore;
	class Renderer;

	typedef Engine::GraphicsSubSystemsTypeList_t::Append<
		CompileTime::TypeListFrom< ShaderEditorCore > >			ShaderEditorSubSystemsTypeList_t;

}	// ShaderEditor

GX_SUBSYSTEM_DECL( ShaderEditor::ShaderEditorSubSystemsTypeList_t, ShaderEditor::ShaderEditorCore );
