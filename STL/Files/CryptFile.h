// Copyright � 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "SubFile.h"
#include "../Containers/StaticArray.h"

namespace GX_STL
{
namespace File
{

	//
	// Simple File Crypt Algorithm
	//

	template <uint Size>
	struct SimpleFileCryptAlgorithm
	{
	// types
	public:
		typedef StaticArray< ubyte, Size >			password_t;
		typedef SimpleFileCryptAlgorithm< Size >	Self;
		
	private:
		struct _RecursiveXor
		{
		private:
			const password_t &	pw;
			usize				pos;
			ubyte	&			c;

		public:
			_RecursiveXor (const password_t &pw, usize p, ubyte &c) : pw(pw), pos(p), c(c)
			{}

			void operator () (usize i)
			{
				c ^= pw[i] + (pos * i);
			}
		};


	// variables
	private:
		password_t	_password;


	// methods
	public:
		SimpleFileCryptAlgorithm (StringCRef password) : _password(0)
		{
			for (usize i = 0; i < password.Length() and i < _password.Count(); ++i) {
				_password[i] = password[i];
			}
		}

		SimpleFileCryptAlgorithm (Buffer<const ubyte> password) : _password(0)
		{
			for (usize i = 0; i < password.Count() and i < _password.Count(); ++i) {
				_password[i] = password[i];
			}
		}

		void Encrypt (usize pos, INOUT ubyte &c) const
		{
			_RecursiveXor	rx( _password, pos, c );
			UnrollLoop< password_t::STATIC_COUNT >::Run( rx );
		}

		void Decrypt (usize pos, INOUT ubyte &c) const	{ Encrypt( pos, c ); }
	};



	//
	// Read only Crypted Sub File
	//

	template <typename A>
	class RCryptFile : public SubRFile
	{
	// types
	public:
		typedef A							CryptAlgorithm;
		typedef RCryptFile<A>				Self;
		typedef SHARED_POINTER_TYPE( Self )	RCryptFilePtr;


	// variables
	private:
		CryptAlgorithm	_crypt;


	// methods
	public:
		RCryptFile (const File::RFilePtr &file, const CryptAlgorithm &alg) :
		  SubRFile( file, file->Pos(), file->RemainingSize() ), _crypt(alg)
		{}


		RCryptFile (const File::RFilePtr &file, Bytes<usize> offset, Bytes<usize> size, const CryptAlgorithm &alg) :
			SubRFile( file, offset, size ), _crypt(alg)
		{}

			
		static RCryptFilePtr New (const File::RFilePtr &file, const CryptAlgorithm &alg)
		{
			return new Self( file, alg );
		}

			
		static RCryptFilePtr New (const File::RFilePtr &file, Bytes<usize> offset, Bytes<usize> size, const CryptAlgorithm &alg)
		{
			return new Self( file, offset, size, alg );
		}

		
		// RFile //
		virtual Bytes<isize> ReadBuf (void * buf, Bytes<usize> size) override
		{
			ubyte *	data = (ubyte *) buf;
			usize	pos  = Pos();

			Bytes<isize> r = SubRFile::ReadBuf( buf, size );

			if ( r > 0 )
			{
				for (usize i = 0; i < usize(r); ++i) {
					_crypt.Decrypt( pos + i, data[i] );
				}
			}

			return r;
		}

		
		// BaseFile //
		virtual EFile::type GetType () const override { return EFile::Crypted; }


	private:
		static SubRFilePtr New (const File::RFilePtr &file, Bytes<usize> offset, Bytes<usize> size)
		{
			return new SubRFile( file, offset, size );
		}
	};



	//
	// Write only Crypted Sub File
	//

	template <typename A>
	class WCryptFile : public SubWFile
	{
	// types
	public:
		typedef A							CryptAlgorithm;
		typedef WCryptFile<A>				Self;
		typedef SHARED_POINTER_TYPE( Self )	WCryptFilePtr;


	// variables
	private:
		CryptAlgorithm	_crypt;
		bool			_restoreData;


	// methods
	public:
		WCryptFile (const File::WFilePtr &file, const CryptAlgorithm &alg, bool restoreData = true) :
			SubWFile( file, file->Pos(), file->RemainingSize() ), _crypt(alg), _restoreData(restoreData)
		{}


		WCryptFile (const File::WFilePtr &file, Bytes<usize> offset, Bytes<usize> size, const CryptAlgorithm &alg, bool restoreData = true) :
			SubWFile( file, offset, size ), _crypt(alg), _restoreData(restoreData)
		{}
		

		static WCryptFilePtr New (const File::WFilePtr &file, const CryptAlgorithm &alg, bool restoreData = true)
		{
			return new Self( file, alg, restoreData );
		}
		

		static WCryptFilePtr New (const File::WFilePtr &file, Bytes<usize> offset, Bytes<usize> size, const CryptAlgorithm &alg, bool restoreData = true)
		{
			return new Self( file, offset, size, alg, restoreData );
		}

			
		// WFile //
		virtual Bytes<isize> WriteBuf (const void * buf, Bytes<usize> size) override
		{
			ubyte *			data = (ubyte *) buf;
			Bytes<usize>	pos  = Pos();

			for (usize i = 0; i < size; ++i) {
				_crypt.Encrypt( pos + i, data[i] );
			}

			Bytes<isize> w = SubWFile::WriteBuf( buf, size );

			if ( _restoreData )
			{
				for (usize i = 0; i < size; ++i) {
					_crypt.Decrypt( pos + i, data[i] );
				}
			}
			return w;
		}

		
		// BaseFile //
		virtual EFile::type GetType () const override { return EFile::Crypted; }


	private:
		static SubWFilePtr New (const File::WFilePtr &file, Bytes<usize> offset, Bytes<usize> size)
		{
			return new SubWFile( file, offset, size );
		}
	};
	

}	// File
}	// GX_STL
