// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "BaseFile.h"

namespace GX_STL
{
namespace File
{

	//
	// Read only Sub File
	//

	class SubRFile : public RFile
	{
	// types
	public:
		SHARED_POINTER( SubRFile );

		typedef SubRFile	Self;


	// variables
	protected:
		RFilePtr	_file;
		isize		_offset,
					_size,
					_pos;


	// methods
	public:
		SubRFile (const File::RFilePtr &file, Bytes<usize> offset, Bytes<usize> size):
			_file(file), _offset(offset), _size(size), _pos(0)
		{
			ASSERT( _file.IsNotNull() );
			ASSERT( offset + size <= _file->Size() );
		}


		static SubRFilePtr New (const File::RFilePtr &file, Bytes<usize> offset, Bytes<usize> size)
		{
			return new Self( file, offset, size );
		}
		

		// RFile //
		virtual Bytes<isize> ReadBuf (void * buf, Bytes<usize> size) override
		{
			Bytes<isize>	result = _file->ReadBufFrom( buf, size, Bytes<usize>( _offset + _pos ) );
			_pos += result;
			return result;
		}
		

		// BaseFile //
		virtual void Close () override
		{
			_file	= null;
			_offset	= 0;
			_size	= 0;
			_pos	= 0;
		}
		
		virtual bool IsOpened () const override
		{
			return _file.IsNotNull();
		}

		virtual bool SeekSet (Bytes<usize> offset) override
		{
			return _SetPos( _offset + offset );
		}

		virtual bool SeekCur (Bytes<isize> offset) override
		{
			return _SetPos( _pos + offset );
		}

		virtual bool SeekEnd (Bytes<usize> offset) override
		{
			return _SetPos( _offset + _size - offset );
		}
		
		virtual Bytes<usize> RemainingSize () const override
		{
			return Bytes<usize>( _size - _pos );
		}

		virtual Bytes<usize> Size () const override
		{
			return Bytes<usize>( _size );
		}

		virtual Bytes<usize> Pos () const override
		{
			return Bytes<usize>( _pos );
		}
		
		virtual bool IsEOF () const override
		{
			return Pos() >= Size();
		}

		virtual StringCRef  Name () const override
		{
			return _file->Name();
		}

		virtual EFile::type  GetType () const override
		{
			return EFile::SubFile;
		}


	private:
		bool _SetPos (isize newPos)
		{
			if ( newPos >= _offset and
				 newPos < _offset + _size )
			{
				_pos = newPos;
				return true;
			}
			else
			{
				_pos = 0;
				return false;
			}
		}
	};

	

	//
	// Write only Sub File
	//

	class SubWFile : public WFile
	{
	// types
	public:
		SHARED_POINTER( SubWFile );

		typedef SubWFile	Self;


	// variables
	protected:
		WFilePtr	_file;
		isize		_offset,
					_size,
					_pos;


	// methods
	public:
		SubWFile (const File::WFilePtr &file, Bytes<usize> offset, Bytes<usize> size):
			_file(file), _offset(offset), _size(size), _pos(0)
		{
			ASSERT( _file.IsNotNull() );
			ASSERT( offset + size <= _file->Size() );
		}


		static SubWFilePtr New (const File::WFilePtr &file, Bytes<usize> offset, Bytes<usize> size)
		{
			return new Self( file, offset, size );
		}
		

		// WFile //
		virtual Bytes<isize> WriteBuf (const void * buf, Bytes<usize> size) override
		{
			const Bytes<usize>	pos		= _file->Pos();
			const Bytes<usize>	new_pos = Bytes<usize>( _offset + _pos );

			if ( pos != new_pos )
				_file->SeekSet( new_pos );

			Bytes<isize>	result = _file->WriteBuf( buf, size );
			_pos += result;

			_file->SeekSet( pos );
			return result;
		}
		
		virtual void Flush () override
		{
			_file->Flush();
		}


		// BaseFile //
		virtual void Close () override
		{
			_file	= null;
			_offset	= 0;
			_size	= 0;
			_pos	= 0;
		}
		
		virtual bool IsOpened () const override
		{
			return _file.IsNotNull();
		}
		
		virtual bool SeekSet (Bytes<usize> offset) override
		{
			return _SetPos( _offset + offset );
		}

		virtual bool SeekCur (Bytes<isize> offset) override
		{
			return _SetPos( _pos + offset );
		}

		virtual bool SeekEnd (Bytes<usize> offset) override
		{
			return _SetPos( _offset + _size - offset );
		}
		
		virtual Bytes<usize> RemainingSize () const override
		{
			return Bytes<usize>( _size - _pos );
		}

		virtual Bytes<usize> Size () const override
		{
			return Bytes<usize>( _size );
		}

		virtual Bytes<usize> Pos () const override
		{
			return Bytes<usize>( _pos );
		}
		
		virtual bool IsEOF () const override
		{
			return Pos() >= Size();
		}

		virtual StringCRef  Name () const override
		{
			return _file->Name();
		}

		virtual EFile::type  GetType () const override
		{
			return EFile::SubFile;
		}


	private:
		bool _SetPos (isize newPos)
		{
			if ( newPos >= _offset and
				 newPos < _offset + _size )
			{
				_pos = newPos;
				return true;
			}
			else
			{
				_pos = 0;
				return false;
			}
		}
	};
	

}	// File
}	// GX_STL
