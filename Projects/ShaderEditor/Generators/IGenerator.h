// Copyright © 2017  Zhirnov Andrey. All rights reserved.
/*
	Multipass resource generator on GPU.
*/

#pragma once

#include "../Core/Common.h"

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
		static IGeneratorPtr  CreateTileLandscapeGenerator (const SubSystemsRef ss);
		static IGeneratorPtr  CreateMicroschemeShader (const SubSystemsRef ss);
	};


}	// ShaderEditor
