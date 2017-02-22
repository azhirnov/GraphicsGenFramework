// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class SinglePass3DGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_mainFunc;
		Array< String >		_files;
		uint				_frame;

		// input/output
		ComputeImagePtr		_outImage;
		TimeD				_globalTime;
		TimeD				_timeDelta;
		float2				_mousePos;
		bool4				_mouseButtons;


	// methods
	public:
		SinglePass3DGenerator (const SubSystemsRef ss) :
			IGenerator( ss ),	_mainFunc( SubSystems() ),
			_frame(0)
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;

		bool Load (uint index);
	};
	
	
/*
=================================================
	Create_SinglePassRayTracing3D
=================================================
*/
	IGeneratorPtr  IGenerator::Create_SinglePassRayTracing3D (const SubSystemsRef ss)
	{
		return New< SinglePass3DGenerator >( ss );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool SinglePass3DGenerator::SetArg (StringCRef name, const VariantRef &arg)
	{
		if ( name == "outImage" and arg.IsType<ComputeImagePtr>() )
		{
			_outImage = arg.Get<ComputeImagePtr>();
			return true;
		}

		if ( name == "globalTime" and arg.IsType<TimeD>() )
		{
			_globalTime = arg.Get<TimeD>();
			return true;
		}

		if ( name == "timeDelta" and arg.IsType<TimeD>() )
		{
			_timeDelta = arg.Get<TimeD>();
			return true;
		}

		if ( name == "mousePos" and arg.IsType<float2>() )
		{
			_mousePos = arg.Get<float2>();
			return true;
		}

		if ( name == "mouseButtons" and arg.IsType<bool4>() )
		{
			_mouseButtons = arg.Get<bool4>();
			return true;
		}

		if ( name == "timeDelta" and arg.IsType<TimeD>() )
		{
			_timeDelta = arg.Get<TimeD>();
			return true;
		}
		
		if ( name == "shader_index" and arg.IsType<uint>() )
		{
			return Load( arg.Get<uint>() );
		}
			
		RETURN_ERR( "Unknown argument name or unsupported type!" );
	}
	
/*
=================================================
	Compile
=================================================
*/
	bool SinglePass3DGenerator::Compile ()
	{
		return _mainFunc.IsCreated();
	}
	
/*
=================================================
	Load
=================================================
*/
	bool SinglePass3DGenerator::Load (uint index)
	{
		struct FileFilter
		{
			bool operator () (StringCRef fname) const
			{
				return	FileAddress::GetExtension( fname ).EqualsIC( "glcs" ) and
						not FileAddress::GetName( fname ).StartsWith( "_" );
			}
		};
		

		if ( _files.Empty() )
		{
			OS::FileSystem::RecursiveFindFiles( "gl/Compute/SinglePassRayTracing3D", FileFilter(), 1, OUT _files );
		}

		if ( index >= _files.Count() )
			return false;

		return _mainFunc.Load( _files[ index ], EShaderCompilationFlags::DefaultCompute );
	}

/*
=================================================
	Render
=================================================
*/
	void SinglePass3DGenerator::Render ()
	{
		CHECK( _outImage );
		
		_mainFunc.SetArg( "outImage",			_outImage );
		_mainFunc.TrySetArg( "unGlobalTime",	(float)_globalTime.Seconds() );
		_mainFunc.TrySetArg( "unTimeDelta",		(float)_timeDelta.Seconds() );
		_mainFunc.TrySetArg( "unFrameIndex",	_frame );
		_mainFunc.TrySetArg( "unMousePos",		_mousePos );
		_mainFunc.TrySetArg( "unMouseButtons",	uint4(_mouseButtons) );

		_mainFunc.Run( _outImage->Dimension().xyo(), uint3(0) );

		++_frame;
	}


}	// ShaderEditor
