// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Containers/Buffer.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Simple Crypt Algorithm (XOR)
	//

	struct SimpleCryptAlgorithm
	{
		// source to encrypted
		template <typename T, typename B>
		static bool Encrypt (Buffer<T> data, Buffer<const B> password)
		{
			FOR( i, data )
			{
				FOR( j, password )
				{
					data[i] ^= ( password[j] + i*j );
				}
			}
			return true;
		}

		// encrypted to source
		template <typename T, typename B>
		static bool Decrypt (Buffer<T> data, Buffer<const B> password)
		{
			return Encrypt( data, password );
		}
	};



	//
	// Crypt
	//
	/*
	template <typename T, typename CryptAlgorithmType = SimpleCryptAlgorithm >
	struct Crypt
	{
	// types
	public:
		typedef CryptAlgorithmType	Algorithm;


	// methods
	public:
	};
	*/


}	// GXTypes
}	// GX_STL
