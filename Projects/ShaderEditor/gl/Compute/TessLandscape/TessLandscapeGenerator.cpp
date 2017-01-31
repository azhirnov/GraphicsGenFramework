// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class TessLandscapeGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_genLandscape;
		ComputeFunction		_genHeight;
		ComputeFunction		_genNormals;
		ComputeFunction		_genDiffuse;

		ComputeImagePtr		_heightImage;
		ComputeImagePtr		_normalsImage;
		uint2				_dimWithBorder;
		const uint2			_imageBorder;

		ComputeImagePtr		_outDiffuseImage;
		SamplerPtr			_diffuseSampler;
		SamplerPtr			_heightSampler;

		// input/output
		TexturePtr			_outDiffuseTex;
		ComputeBufferPtr	_inBuffer;
		ComputeBufferPtr	_outBuffer;
		uint2				_dimension;
		uint				_numVertices;
		float2				_position;
		float				_scale;


	// methods
	public:
		TessLandscapeGenerator (const SubSystemsRef ss) :
			IGenerator( ss ), _numVertices(0), _scale(1.0f), _imageBorder(2)
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;
	};
	
	
/*
=================================================
	Create_TessLandscape
=================================================
*/
	IGeneratorPtr  IGenerator::Create_TessLandscape (const SubSystemsRef ss)
	{
		return BaseObject::_New( new TessLandscapeGenerator( ss ) );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool TessLandscapeGenerator::SetArg (StringCRef name, const VariantRef &arg)
	{
		if ( name == "inBuffer" and arg.IsType<ComputeBufferPtr>() )
		{
			_inBuffer = arg.Get< ComputeBufferPtr >();
			return true;
		}

		if ( name == "outBuffer" and arg.IsType<ComputeBufferPtr>() )
		{
			_outBuffer = arg.Get< ComputeBufferPtr >();
			return true;
		}

		if ( name == "numVertices" and arg.IsType<uint>() )
		{
			_numVertices = arg.Get<uint>();
			return true;
		}

		if ( name == "dimension" and arg.IsType<uint2>() )
		{
			_dimension		= arg.Get<uint2>();
			_dimWithBorder	= _dimension + _imageBorder * 2;
			return true;
		}

		if ( name == "position" and arg.IsType<float2>() )
		{
			_position = arg.Get<float2>();
			return true;
		}

		if ( name == "scale" and arg.IsType<float>() )
		{
			_scale = arg.Get<float>();
			return true;
		}

		if ( name == "outDiffuseTex" and arg.IsType<TexturePtr>() )
		{
			_outDiffuseTex   = arg.Get<TexturePtr>();
			_outDiffuseImage = ComputeImage::New( _outDiffuseTex ); 
			return true;
		}

		RETURN_ERR( "Unknown argument name or unsupported type!" );
	}
	
/*
=================================================
	Compile
=================================================
*/
	bool TessLandscapeGenerator::Compile ()
	{
		CHECK_ERR( _genLandscape.Load( SubSystems(), "gl/Generators/TessLandscape/gen_landscape.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _genNormals.Load( SubSystems(), "gl/Generators/TessLandscape/gen_normals.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _genDiffuse.Load( SubSystems(), "gl/Generators/TessLandscape/gen_diffuse.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _genHeight.Load( SubSystems(), "gl/Generators/TessLandscape/gen_height.glcs", EShaderCompilationFlags::DefaultCompute ) );

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::Anisotropic_16 ), OUT _diffuseSampler ) );

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::MinMagMipLinear ), OUT _heightSampler ) );

		_heightImage	= ComputeImage::New( SubSystems() );
		_normalsImage	= ComputeImage::New( SubSystems() );

		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	void TessLandscapeGenerator::Render ()
	{
		CHECK( _genLandscape.IsCreated() );
		CHECK( _genHeight.IsCreated() );
		CHECK( _genNormals.IsCreated() );
		CHECK( IsNotZero( _dimension ) );

		CHECK( _inBuffer and _outBuffer and _outDiffuseTex );
		CHECK( _inBuffer->Size() == _outBuffer->Size() );

		if ( Any( _dimWithBorder != _heightImage->Dimension().xy() ) or
			 Any( _dimWithBorder != _normalsImage->Dimension().xy() ) )
		{
			CHECK( _heightImage->Create( _dimWithBorder.xyoo(), ETexture::Tex2D, EPixelFormat::R32F ) );
			CHECK( _normalsImage->Create( _dimWithBorder.xyoo(), ETexture::Tex2D, EPixelFormat::RGBA16F ) );
		
			_heightImage->GetSharedObject()->SetSampler( _heightSampler );
			_normalsImage->GetSharedObject()->SetSampler( _heightSampler );
		}
		
		const float2	scale_with_border	= float2( _scale ) * float2(_dimWithBorder) / float2(_dimension);
		const float4	img_scale_bias		= uint4( _dimension-1, _imageBorder ).To<float4>();
		const float4	texc_scale_bias		= img_scale_bias / float2(_dimWithBorder-1).xyxy();


		// generate height map
		_genHeight.SetArg( "outHeightImage",	_heightImage );
		_genHeight.SetArg( "unPosition",		_position );
		_genHeight.SetArg( "unScale",			scale_with_border );

		_genHeight.Run( _heightImage->Dimension().xyo() );


		// generate normals map
		_genNormals.SetArg( "inHeightImage",	_heightImage );
		_genNormals.SetArg( "outNormalsImage",	_normalsImage );
		_genNormals.SetArg( "unScale",			_scale / float2(_dimWithBorder) );
		
		_genNormals.Run( _normalsImage->Dimension().xyo() );


		// copy height and normals to vertex buffer
		_genLandscape.SetArg( "inBuffer",			_inBuffer );
		_genLandscape.SetArg( "outBuffer",			_outBuffer );
		_genLandscape.SetArg( "inHeightImage",		_heightImage );
		_genLandscape.SetArg( "inNormalsImage",		_normalsImage );
		_genLandscape.SetArg( "unImageScaleBias",	img_scale_bias );

		_genLandscape.Run( uint3( _numVertices, 0, 0 ) );

		
		// generate diffuse map
		_genDiffuse.SetArg( "outDiffuseImage",		_outDiffuseImage );
		_genDiffuse.SetArg( "inHeightTexture",		_heightImage->GetSharedObject() );
		_genDiffuse.SetArg( "inNormalsTexture",		_normalsImage->GetSharedObject() );
		_genDiffuse.SetArg( "unTextureScaleBias",	texc_scale_bias );
		//_genDiffuse.SetArg( "unPosition",		_position );
		//_genDiffuse.SetArg( "unScale",			scale_with_border );

		_genDiffuse.Run( _outDiffuseImage->Dimension().xyo() );
		
		_outDiffuseTex->GenerateMipmaps();
		_outDiffuseTex->SetSampler( _diffuseSampler );
	}


}	// ShaderEditor
