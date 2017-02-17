// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class VoronoiRecursionGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_mainFunc;

		// input/output
		ComputeImagePtr		_outImage;
		TimeD				_globalTime;


	// methods
	public:
		VoronoiRecursionGenerator (const SubSystemsRef ss) :
			IGenerator( ss ), _mainFunc( SubSystems() )
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;
	};
	
	
/*
=================================================
	Create_VoronoiRecursion
=================================================
*/
	IGeneratorPtr  IGenerator::Create_VoronoiRecursion (const SubSystemsRef ss)
	{
		return New<VoronoiRecursionGenerator>( ss );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool VoronoiRecursionGenerator::SetArg (StringCRef name, const VariantRef &arg)
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
			
		RETURN_ERR( "Unknown argument name or unsupported type!" );
	}
	
/*
=================================================
	Compile
=================================================
*/
	bool VoronoiRecursionGenerator::Compile ()
	{
		CHECK_ERR( _mainFunc.Load( "gl/Compute/VoronoiRecursion/main.glcs", EShaderCompilationFlags::DefaultCompute ) );

		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	void VoronoiRecursionGenerator::Render ()
	{
		CHECK( _outImage );
		
		_mainFunc.SetArg( "outImage",		_outImage );
		_mainFunc.SetArg( "unGlobalTime",	(float)_globalTime.Seconds() );

		_mainFunc.Run( _outImage->Dimension().xyo(), uint3(0) );
	}


}	// ShaderEditor
