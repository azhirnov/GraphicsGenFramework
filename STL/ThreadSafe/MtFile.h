// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Files/BaseFile.h"

namespace GX_STL
{
namespace File
{

	//
	// Read only multithread file
	//

	class MtRFile : public RFile
	{
	// variables
	private:
		RFilePtr		_file;
		mutable Mutex	_lock;


	// methods
	public:
		explicit
		MtRFile (const RFilePtr &file) : _file(file)
		{}


		static SHARED_POINTER_TYPE( MtRFile )  New (const RFilePtr &file)
		{
			ASSERT( file->GetType() != EFile::Multithreaded );
			return new MtRFile( file );
		}

		
		// RFile //
		virtual Bytes<isize> ReadBuf (void * buf, Bytes<usize> size) override
		{
			SCOPELOCK( _lock );
			return _file->ReadBuf( buf, size );
		}
		
		virtual Bytes<isize> ReadBufFrom (void * buf, Bytes<usize> size, Bytes<usize> offset) override
		{
			SCOPELOCK( _lock );

			const Bytes<usize>	pos = _file->Pos();
			_file->SeekSet( offset );

			const Bytes<isize>	result = _file->ReadBuf( buf, size );
			_file->SeekSet( pos );
			
			return result;
		}


		// BaseFile //
		virtual void Close () override
		{
			SCOPELOCK( _lock );
			_file->Close();
		}
		
		virtual bool IsOpened () const override
		{
			SCOPELOCK( _lock );
			return _file.IsNotNull();
		}

		virtual bool SeekSet (Bytes<usize> offset) override
		{
			SCOPELOCK( _lock );
			return _file->SeekSet( offset );
		}

		virtual bool SeekCur (Bytes<isize> offset) override
		{
			SCOPELOCK( _lock );
			return _file->SeekCur( offset );
		}

		virtual bool SeekEnd (Bytes<usize> offset) override
		{
			SCOPELOCK( _lock );
			return _file->SeekEnd( offset );
		}
		
		virtual Bytes<usize> RemainingSize () const override
		{
			SCOPELOCK( _lock );
			return _file->RemainingSize();
		}

		virtual Bytes<usize> Size () const override
		{
			SCOPELOCK( _lock );
			return _file->Size();
		}

		virtual Bytes<usize> Pos () const override
		{
			SCOPELOCK( _lock );
			return _file->Pos();
		}
		
		virtual bool IsEOF () const override
		{
			SCOPELOCK( _lock );
			return _file->IsEOF();
		}

		virtual StringCRef  Name () const override
		{
			SCOPELOCK( _lock );
			return _file->Name();
		}

		virtual EFile::type  GetType () const override
		{
			return EFile::Multithreaded;
		}
	};

	

	//
	// Write only multithread file
	//

	class MtWFile : public WFile
	{
	// variables
	private:
		WFilePtr		_file;
		mutable Mutex	_lock;


	// methods
	public:
		MtWFile (const WFilePtr &file) : _file(file)
		{}


		static SHARED_POINTER_TYPE( MtWFile )  New (const WFilePtr &file)
		{
			ASSERT( file->GetType() != EFile::Multithreaded );
			return new MtWFile( file );
		}

		
		// WFile //
		virtual Bytes<isize> WriteBuf (const void * buf, Bytes<usize> size) override
		{
			SCOPELOCK( _lock );
			return _file->WriteBuf( buf, size );
		}

		virtual void Flush () override
		{
			SCOPELOCK( _lock );
			return _file->Flush();
		}
		

		// BaseFile //
		virtual void Close () override
		{
			SCOPELOCK( _lock );
			_file->Close();
		}
		
		virtual bool IsOpened () const override
		{
			SCOPELOCK( _lock );
			return _file.IsNotNull();
		}

		virtual bool SeekSet (Bytes<usize> offset) override
		{
			SCOPELOCK( _lock );
			return _file->SeekSet( offset );
		}

		virtual bool SeekCur (Bytes<isize> offset) override
		{
			SCOPELOCK( _lock );
			return _file->SeekCur( offset );
		}

		virtual bool SeekEnd (Bytes<usize> offset) override
		{
			SCOPELOCK( _lock );
			return _file->SeekEnd( offset );
		}
		
		virtual Bytes<usize> RemainingSize () const override
		{
			SCOPELOCK( _lock );
			return _file->RemainingSize();
		}

		virtual Bytes<usize> Size () const override
		{
			SCOPELOCK( _lock );
			return _file->Size();
		}

		virtual Bytes<usize> Pos () const override
		{
			SCOPELOCK( _lock );
			return _file->Pos();
		}
		
		virtual bool IsEOF () const override
		{
			SCOPELOCK( _lock );
			return _file->IsEOF();
		}

		virtual StringCRef  Name () const override
		{
			SCOPELOCK( _lock );
			return _file->Name();
		}

		virtual EFile::type  GetType () const override
		{
			return EFile::Multithreaded;
		}
	};


}	// File
}	// GX_STL
