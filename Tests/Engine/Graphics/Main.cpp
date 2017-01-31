// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/Graphics/Engine.Graphics.h"

using namespace Engine;
using namespace Engine::Base;
using namespace Engine::Graphics;


class TestApplication : public GameUtils::GameApplication
{
// variables
private:
	GraphicsEngine			graphics;
	ShaderProgramPtr		program;
	MeshGenerator::Mesh		quadMesh;


// methods
public:
	TestApplication (const IParallelThreadPtr &thread) :
		GameApplication( thread ), graphics( SubSystems() )
	{
		CHECK( SubSystems()->Get< FileManager >()->FindAndSetCurrentDir( "Tests/Engine/Graphics" ) );

		CHECK( Logger::GetInstance()->Open( "log", false ) );
	}


	~TestApplication ()
	{
	}


	// GameApplication //
	void _Update (TimeD dt, bool forceRedraw)
	{
		//CPUTIME_PROFILER("1");
		//GPUTIME_PROFILER("");

		program->Bind();

		quadMesh.Draw();

		graphics.FlushFrame();
	}


	void _OnInit ()
	{
		CHECK( SubSystems()->Get< Platform >()->
			InitWindow( Platform::WindowDesc( "Test", int2(800, 600), int2(MinValue<int>()), false, true ) ) );

		CHECK( SubSystems()->Get< Platform >()->
			InitRender( VideoSettings(	VideoSettings::RGBA8,
										VideoSettings::DEPTH_24,
										VideoSettings::NO_STENCIL,
										0,
										VideoSettings::AUTO,
										VideoSettings::CONTEXT_NO_ERROR,
										true,
										false ) ) );
	}


	void _OnSurfaceCreated ()
	{
		graphics.Initialize();

		graphics.GetShaderManager()->SetDebugOutputFolder( "debug_shader" );
		
		//bool no_error = graphics.GetContext()->IsExtensionSupported( "GL_KHR_no_error" );

		quadMesh = MeshGenerator::CreateQuad( SubSystems(), RectF( -1.0f, -1.0f, 1.0f, 1.0f ), RectF( 0.0f, 0.0f, 1.0f, 1.0f ) );

		program = ShaderProgram::New( SubSystems() );

		CHECK( program->Load( "prog.glsl",
							  ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment ),
							  EShaderCompilationFlags::Default,
							  quadMesh.GetVertexAttribs()->GetAttribsState(),
							  FragmentOutputState().Set( EFragOutput::Float4, ERenderTarget::Color0 ) ) );
	}


	void _OnExit ()
	{
		graphics.Deinitialize();
	}


	void _OnResized ()
	{
	}
};

MAIN_APPLICATION_ENTRY( TestApplication );
