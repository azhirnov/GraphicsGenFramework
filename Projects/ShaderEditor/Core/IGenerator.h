// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Multipass resource generator on GPU (compute shader).
*/

#pragma once

#include "Common.h"

namespace ShaderEditor
{
	
	class IGenerator;
	SHARED_POINTER( IGenerator );


	//
	// Generator interface
	//

	class IGenerator : public BaseObject
	{
	// methods
	protected:
		IGenerator (const SubSystemsRef ss) : BaseObject( ss )
		{}

		// TODO
		//ComputeImagePtr			_FindImage (StringCRef name) const;
		//TexturePtr				_FindTexture (StringCRef name) const;
		//ComputeBufferPtr		_FindBuffer (StringCRef name) const;


	// interface
	public:
		virtual bool SetArg (StringCRef name, const VariantRef &arg) = 0;

		virtual bool Compile () = 0;

		virtual void Render () = 0;


	// constructors
	public:
		static IGeneratorPtr  Create_SimpleLandscape (const SubSystemsRef ss);
		static IGeneratorPtr  Create_TessLandscape (const SubSystemsRef ss);
		static IGeneratorPtr  Create_Microscheme (const SubSystemsRef ss);
		static IGeneratorPtr  Create_ThousandsOfStars (const SubSystemsRef ss);
		static IGeneratorPtr  Create_SimpleParticles (const SubSystemsRef ss);
		static IGeneratorPtr  Create_InteractiveParticles (const SubSystemsRef ss);
		static IGeneratorPtr  Create_SinglePassRayTracing2D (const SubSystemsRef ss);
		static IGeneratorPtr  Create_SinglePassRayTracing3D (const SubSystemsRef ss);
	};


}	// ShaderEditor
