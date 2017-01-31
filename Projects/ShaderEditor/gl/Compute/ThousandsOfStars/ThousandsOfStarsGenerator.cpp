// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class ThousandsOfStarsGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_pass1Func;
		ComputeFunction		_pass2Func;

		ComputeImagePtr		_tempImage;
		SamplerPtr			_sampler;

		// input/output
		ComputeImagePtr		_outImage;
		TimeD				_globalTime;


	// methods
	public:
		ThousandsOfStarsGenerator (const SubSystemsRef ss) :
			IGenerator( ss )
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;
	};
	
	
/*
=================================================
	Create_ThousandsOfStars
=================================================
*/
	IGeneratorPtr  IGenerator::Create_ThousandsOfStars (const SubSystemsRef ss)
	{
		return BaseObject::_New( new ThousandsOfStarsGenerator( ss ) );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool ThousandsOfStarsGenerator::SetArg (StringCRef name, const VariantRef &arg)
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
	bool ThousandsOfStarsGenerator::Compile ()
	{
		CHECK_ERR( _pass1Func.Load( SubSystems(), "gl/Compute/ThousandsOfStars/pass1.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _pass2Func.Load( SubSystems(), "gl/Compute/ThousandsOfStars/pass2.glcs", EShaderCompilationFlags::DefaultCompute ) );
		
		_tempImage = ComputeImage::New( SubSystems() );

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::MinMagMipLinear ), OUT _sampler ) );
		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	void ThousandsOfStarsGenerator::Render ()
	{
		CHECK( _outImage );
		
		if ( Any( _outImage->Dimension().xy() != _tempImage->Dimension().xy() ) )
		{
			CHECK( _tempImage->Create( _outImage->Dimension(), ETexture::Tex2D, EPixelFormat::RGBA8_UNorm ) );

			_tempImage->GetSharedObject()->SetSampler( _sampler );
		}

		// generate main image
		_pass1Func.SetArg( "outImage",		_tempImage );
		_pass1Func.SetArg( "unGlobalTime",	(float)_globalTime.Seconds() );
		_pass1Func.Run( _tempImage->Dimension().xyo(), uint3(0) );


		// post effects
		_pass2Func.SetArg( "outImage",		_outImage );
		_pass2Func.SetArg( "inTexture",		_tempImage->GetSharedObject() );
		_pass2Func.Run( _outImage->Dimension().xyo(), uint3(0) );
	}


}	// ShaderEditor
