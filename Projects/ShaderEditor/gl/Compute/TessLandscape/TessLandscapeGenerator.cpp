// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class TessLandscapeGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_genLandscape;
		ComputeFunction		_genDiffuse;
		ComputeFunction		_genDisplacement;

		const uint2			_imageBorder;

		ComputeImagePtr		_outDiffuseImage;
		ComputeImagePtr		_outDisplacementImage;
		SamplerPtr			_diffuseSampler;
		SamplerPtr			_heightSampler;

		// input/output
		TexturePtr			_outDiffuseTex;
		TexturePtr			_outDisplacementMap;
		ComputeBufferPtr	_inBuffer;
		ComputeBufferPtr	_outBuffer;
		uint2				_dimension;
		uint				_numVertices;
		float2				_position;
		float				_scale;


	// methods
	public:
		TessLandscapeGenerator (const SubSystemsRef ss) :
			IGenerator( ss ),
			_genLandscape( SubSystems() ),		_genDiffuse( SubSystems() ),
			_genDisplacement( SubSystems() ),	_numVertices(0),
			_scale(1.0f)
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
		return New<TessLandscapeGenerator>( ss );
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
			_dimension = arg.Get<uint2>();
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

		if ( name == "outDisplacementMap" and arg.IsType<TexturePtr>() )
		{
			_outDisplacementMap   = arg.Get<TexturePtr>();
			_outDisplacementImage = ComputeImage::New( _outDisplacementMap ); 
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
		CHECK_ERR( _genDisplacement.Load( "gl/Compute/TessLandscape/gen_displacement.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _genLandscape.Load( "gl/Compute/TessLandscape/gen_landscape.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _genDiffuse.Load( "gl/Compute/TessLandscape/gen_diffuse.glcs", EShaderCompilationFlags::DefaultCompute ) );

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::Anisotropic_16 ), OUT _diffuseSampler ) );

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::MinMagMipLinear ), OUT _heightSampler ) );

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
		CHECK( _genDisplacement.IsCreated() );
		CHECK( IsNotZero( _dimension ) );

		CHECK( _inBuffer and _outBuffer and _outDiffuseTex );
		CHECK( _inBuffer->Size() == _outBuffer->Size() );


		// generate displacement (height) map
		_genDisplacement.SetArg( "outDisplacementImage",	_outDisplacementImage );
		_genDisplacement.SetArg( "unPosition",				_position );
		_genDisplacement.SetArg( "unScale",					float2(_scale) );
		_genDisplacement.SetArg( "unInvSize",				1.0f / float2(_outDisplacementImage->Dimension().xy() - 1) );

		_genDisplacement.Run( _outDisplacementImage->Dimension().xyo() );


		// copy height and normals to vertex buffer
		_genLandscape.SetArg( "inBuffer",		_inBuffer );
		_genLandscape.SetArg( "outBuffer",		_outBuffer );
		_genLandscape.SetArg( "unPosition",		_position );
		_genLandscape.SetArg( "unScale",		float2(_scale) );
		_genLandscape.SetArg( "unInvSize",		1.0f / float2(_dimension - 1) );

		_genLandscape.Run( uint3( _numVertices, 0, 0 ) );

		
		// generate diffuse map
		_genDiffuse.SetArg( "outDiffuseImage",	_outDiffuseImage );
		_genDiffuse.SetArg( "unPosition",		_position );
		_genDiffuse.SetArg( "unScale",			float2(_scale) );

		_genDiffuse.Run( _outDiffuseImage->Dimension().xyo() );
		

		// prepare output textures
		_outDiffuseTex->GenerateMipmaps();
		_outDiffuseTex->SetSampler( _diffuseSampler );

		_outDisplacementMap->GenerateMipmaps();
		_outDisplacementMap->SetSampler( _heightSampler );
	}


}	// ShaderEditor
