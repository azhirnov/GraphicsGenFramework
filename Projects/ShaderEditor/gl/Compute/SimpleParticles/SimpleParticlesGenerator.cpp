// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class SimpleParticlesGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_initFunc;
		ComputeFunction		_updateFunc;
		TimeD				_prevGlobalTime;

		// input/output
		VertexBufferPtr		_inBuffer;
		VertexBufferPtr		_outBuffer;
		TimeD				_globalTime;
		uint				_count;
		bool				_initialize;


	// methods
	public:
		SimpleParticlesGenerator (const SubSystemsRef ss) :
			IGenerator( ss ), _count(0), _initialize(false)
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;
	};
	
	
/*
=================================================
	Create_SimpleParticles
=================================================
*/
	IGeneratorPtr  IGenerator::Create_SimpleParticles (const SubSystemsRef ss)
	{
		return BaseObject::_New( new SimpleParticlesGenerator( ss ) );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool SimpleParticlesGenerator::SetArg (StringCRef name, const VariantRef &arg)
	{
		if ( name == "inBuffer" and arg.IsType<VertexBufferPtr>() )
		{
			_inBuffer = arg.Get<VertexBufferPtr>();
			return true;
		}

		if ( name == "outBuffer" and arg.IsType<VertexBufferPtr>() )
		{
			_outBuffer = arg.Get<VertexBufferPtr>();
			return true;
		}

		if ( name == "globalTime" and arg.IsType<TimeD>() )
		{
			_globalTime = arg.Get<TimeD>();
			return true;
		}

		if ( name == "initialize" and arg.IsType<bool>() )
		{
			_initialize = arg.Get<bool>();
			return true;
		}

		if ( name == "count" and arg.IsType<uint>() )
		{
			_count = arg.Get<uint>();
			return true;
		}
			
		RETURN_ERR( "Unknown argument name or unsupported type!" );
	}
	
/*
=================================================
	Compile
=================================================
*/
	bool SimpleParticlesGenerator::Compile ()
	{
		CHECK_ERR( _initFunc.Load( SubSystems(), "gl/Compute/SimpleParticles/init.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _updateFunc.Load( SubSystems(), "gl/Compute/SimpleParticles/update.glcs", EShaderCompilationFlags::DefaultCompute ) );
		
		_count		= 0;
		_initialize	= true;
		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	void SimpleParticlesGenerator::Render ()
	{
		CHECK( _inBuffer and _outBuffer );
		CHECK( _count > 0 );


		if ( _initialize )
		{
			_initFunc.SetArg( "outBuffer",	_outBuffer );

			_initFunc.Run( uint3(_count, 0, 0) );

			_initialize		= false;
			_prevGlobalTime = _globalTime;
			return;
		}

		
		TimeD	dt = _globalTime - _prevGlobalTime;

		_prevGlobalTime = _globalTime;

		if ( dt.IsZero() )
			return;

		_updateFunc.SetArg( "inBuffer",		_inBuffer );
		_updateFunc.SetArg( "outBuffer",	_outBuffer );
		_updateFunc.SetArg( "unTimeDelta",	(float)dt.Seconds() );

		_updateFunc.Run( uint3(_count, 0, 0) );
	}


}	// ShaderEditor
