// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#include "../IGenerator.h"

namespace ShaderEditor
{

	class MicroschemeShader : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_createGridFunc;
		ComputeFunction		_mainFunc;

		ComputeImagePtr		_gridImage;
		uint2				_cellsCount;

		// input/output
		ComputeImagePtr		_outImage;
		uint2				_cellSize;
		uint2				_subCellSize;
		Time<double>		_globalTime;


	// methods
	public:
		MicroschemeShader (const SubSystemsRef ss) :
			IGenerator( ss )
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;

		void UpdateCellsCache ();
	};
	
	
/*
=================================================
	CreateMicroschemeShader
=================================================
*/
	IGeneratorPtr  IGenerator::CreateMicroschemeShader (const SubSystemsRef ss)
	{
		return BaseObject::_New( new MicroschemeShader( ss ) );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool MicroschemeShader::SetArg (StringCRef name, const VariantRef &arg)
	{
		if ( name == "outImage" and arg.IsType<ComputeImagePtr>() )
		{
			_outImage = arg.Get<ComputeImagePtr>();
			return true;
		}

		if ( name == "cellSize" and arg.IsType<uint2>() )
		{
			_cellSize = arg.Get<uint2>();
			return true;
		}

		if ( name == "subCellSize" and arg.IsType<uint2>() )
		{
			_subCellSize = arg.Get<uint2>();
			return true;
		}

		if ( name == "globalTime" and arg.IsType<Time<double>>() )
		{
			_globalTime = arg.Get< Time<double> >();
			return true;
		}
			
		RETURN_ERR( "Unknown argument name or unsupported type!" );
	}
	
/*
=================================================
	Compile
=================================================
*/
	bool MicroschemeShader::Compile ()
	{
		CHECK_ERR( _createGridFunc.Load( SubSystems(), "Generators/Microscheme/create_grid.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _mainFunc.Load( SubSystems(), "Generators/Microscheme/main.glcs", EShaderCompilationFlags::DefaultCompute ) );

		_gridImage = ComputeImage::New( SubSystems() );
		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	void MicroschemeShader::Render ()
	{
		CHECK( IsNotZero( _cellSize ) and IsNotZero( _subCellSize ) );
		CHECK( _outImage.IsNotNull() );

		UpdateCellsCache();
		
		_mainFunc.SetArg( "outImage",			_outImage );
		_mainFunc.SetArg( "inGridDataImage",	_gridImage );
		_mainFunc.SetArg( "unCellSize",			_cellSize );
		_mainFunc.SetArg( "unGlobalTime",		(float)_globalTime.Seconds() );

		_mainFunc.Run( _outImage->Dimension().xyo(), uint3(0) );
	}
	
/*
=================================================
	UpdateCellsCache
=================================================
*/
	void MicroschemeShader::UpdateCellsCache ()
	{
		_cellsCount = _outImage->Dimension().xy() / (_cellSize * 10);	// TODO

		if ( Any( _cellsCount * _cellSize > _gridImage->Dimension().xy() ) )
		{
			CHECK( _gridImage->Create( 
				(_cellsCount * _cellSize).xyoo(),
				ETexture::Tex2D,
				EPixelFormat::RGBA8U ) );
			
			_createGridFunc.SetArg( "outGridDataImage",	_gridImage );
			_createGridFunc.SetArg( "unMaxSubCellSize",	_subCellSize );
			_createGridFunc.SetArg( "unCellSize",		_cellSize );

			_createGridFunc.Run( _cellsCount.xyo() );
		}
	}


}	// ShaderEditor
