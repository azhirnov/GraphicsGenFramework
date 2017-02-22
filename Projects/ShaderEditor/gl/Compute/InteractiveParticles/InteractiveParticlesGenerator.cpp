// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IGenerator.h"

namespace ShaderEditor
{

	class InteractiveParticlesGenerator : public IGenerator
	{
	// variables
	private:
		ComputeFunction		_initFunc;
		ComputeFunction		_updateFunc;

		// input/output
		VertexBufferPtr		_outBuffer;
		TimeD				_globalTime;
		TimeD				_timeDelta;
		float4				_mouse;
		uint				_count;
		bool				_initialize;


	// methods
	public:
		InteractiveParticlesGenerator (const SubSystemsRef ss) :
			IGenerator( ss ),
			_initFunc( SubSystems() ),	_updateFunc( SubSystems() ),
			_count(0), _initialize(false)
		{}

		bool SetArg (StringCRef name, const VariantRef &arg) override;

		bool Compile () override;

		void Render () override;
	};
	
	
/*
=================================================
	Create_InteractiveParticles
=================================================
*/
	IGeneratorPtr  IGenerator::Create_InteractiveParticles (const SubSystemsRef ss)
	{
		return New< InteractiveParticlesGenerator >( ss );
	}

/*
=================================================
	SetArg
=================================================
*/
	bool InteractiveParticlesGenerator::SetArg (StringCRef name, const VariantRef &arg)
	{
		if ( name == "buffer" and arg.IsType<VertexBufferPtr>() )
		{
			_outBuffer = arg.Get<VertexBufferPtr>();
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

		if ( name == "mouse" and arg.IsType<float4>() )
		{
			_mouse = arg.Get<float4>();
			return true;
		}
			
		RETURN_ERR( "Unknown argument name or unsupported type!" );
	}
	
/*
=================================================
	Compile
=================================================
*/
	bool InteractiveParticlesGenerator::Compile ()
	{
		CHECK_ERR( _initFunc.Load( "gl/Compute/InteractiveParticles/init.glcs", EShaderCompilationFlags::DefaultCompute ) );
		CHECK_ERR( _updateFunc.Load( "gl/Compute/InteractiveParticles/update.glcs", EShaderCompilationFlags::DefaultCompute ) );
		
		_count		= 0;
		_initialize	= true;
		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	void InteractiveParticlesGenerator::Render ()
	{
		CHECK( _outBuffer );
		CHECK( _count > 0 );

		if ( _initialize )
		{
			_initFunc.SetArg( "outBuffer",		_outBuffer );
			_initFunc.SetArg( "unGlobalTime",	(float)_globalTime.Seconds() );

			_initFunc.Run( uint3(_count, 0, 0) );

			_initialize = false;
			return;
		}

		_updateFunc.SetArg( "outBuffer",	_outBuffer );
		_updateFunc.SetArg( "unTimeDelta",	(float)_timeDelta.Seconds() );
		_updateFunc.SetArg( "unGlobalTime",	(float)_globalTime.Seconds() );
		_updateFunc.SetArg( "unMouse",		_mouse );

		_updateFunc.Run( uint3(_count, 0, 0) );
	}


}	// ShaderEditor
