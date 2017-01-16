// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "BaseFile.h"

namespace GX_STL
{
namespace File
{

	//
	// HDD Read only File
	//

	class HddRFile : public RFile
	{
	// types
	public:
		SHARED_POINTER( HddRFile );


	// variables
	protected:
		Ptr< FILE >		_file;
		String			_name;


	// methods
	public:
		HddRFile ()		{}
		~HddRFile ()	{ _Close(); }


		static HddRFilePtr New ()
		{
			return new HddRFile();
		}

		static HddRFilePtr New (StringCRef address)
		{
			HddRFilePtr	file = new HddRFile();

			if ( file->Open( address ) )
				return file;

			return null;
		}


		bool Open (StringCRef address)
		{
			Close();
			_name = address;
			_file = fopen( address.cstr(), "rb" );
			return IsOpened();
		}


		// RFile //
		virtual Bytes<isize> ReadBuf (void * buf, Bytes<usize> size) override
		{
			return Bytes<isize>( fread( buf, 1, size, _file ) );
		}


		// BaseFile //
		virtual void Close () override
		{
			return _Close();
		}

		virtual bool IsOpened () const override
		{
			return _file.IsNotNull();
		}

		virtual bool SeekSet (Bytes<usize> offset) override
		{
			return fseek( _file, (long)offset, SEEK_SET ) == 0;
		}

		virtual bool SeekCur (Bytes<isize> offset) override
		{
			return fseek( _file, (long)offset, SEEK_CUR ) == 0;
		}

		virtual bool SeekEnd (Bytes<usize> offset) override
		{
			return fseek( _file, (long)offset, SEEK_END ) == 0;
		}
		
		virtual Bytes<usize> RemainingSize () const override
		{
			usize size = 0;
			usize pos  = ftell( _file );

			fseek( _file, 0, SEEK_END );
			size = ftell( _file );
			fseek( _file, (long)pos, SEEK_SET );

			return Bytes<usize>( size - pos );
		}

		virtual Bytes<usize> Size () const override
		{
			usize size = 0;
			usize pos  = ftell( _file );

			fseek( _file, 0, SEEK_END );
			size = ftell( _file );
			fseek( _file, (long)pos, SEEK_SET );

			return Bytes<usize>( size );
		}

		virtual Bytes<usize> Pos () const override
		{
			return Bytes<usize>( ftell( _file ) );
		}
		
		virtual bool IsEOF () const override
		{
			return Pos() >= Size();
		}

		virtual StringCRef  Name () const override
		{
			return _name;
		}

		virtual EFile::type  GetType () const override
		{
			return EFile::HDD;
		}


	private:
		void _Close ()
		{
			if ( IsOpened() )
			{
				fclose( _file );
				_file = null;
			}
			_name.Clear();
		}
	};



	//
	// HDD Write only File
	//

	class HddWFile : public WFile
	{
	// types
	public:
		struct EOpenFlags
		{
			enum type
			{
				None	= 0,		// create empty file
				Append	= 1 << 0,	// write to end, seek ignored, create if not exist
				Update	= 1 << 1,	// create empty file for input and output

				AppendUpdate = Append | Update,	// write to end
			};
		};

		SHARED_POINTER( HddWFile );


	// variables
	protected:
		Ptr< FILE >		_file;
		String			_name;


	// methods
	public:
		HddWFile ()		{}
		~HddWFile ()	{ _Close(); }
		

		static HddWFilePtr New ()
		{
			return new HddWFile();
		}

		static HddWFilePtr New (StringCRef address, int flags = EOpenFlags::None)
		{
			HddWFilePtr	file = new HddWFile();

			if ( file->Open( address, flags ) )
				return file;

			return null;
		}


		bool Open (StringCRef address, int flags = EOpenFlags::None)
		{
			char	mode[8]	= {0};
			int		pos		= 0;

			if ( EnumEq( flags, EOpenFlags::Append ) )
				mode[ pos++ ] = 'a';
			else
				mode[ pos++ ] = 'w';

			mode[ pos++ ] = 'b';

			if ( EnumEq( flags, EOpenFlags::Update ) )
				mode[ pos++ ] = '+';

			mode[ pos++ ] = 0;


			Close();
			_name = address;
			_file = fopen( address.cstr(), mode );
			return IsOpened();
		}


		// WFile //
		virtual Bytes<isize> WriteBuf (const void * buf, Bytes<usize> size) override
		{
			return Bytes<isize>( fwrite( buf, 1, size, _file ) );
		}
		
		virtual void Flush () override
		{
			fflush( _file );
		}


		// BaseFile //
		virtual void Close () override
		{
			return _Close();
		}

		virtual bool IsOpened () const override
		{
			return _file.IsNotNull();
		}

		virtual bool SeekSet (Bytes<usize> offset) override
		{
			return fseek( _file, (long)offset, SEEK_SET ) == 0;
		}

		virtual bool SeekCur (Bytes<isize> offset) override
		{
			return fseek( _file, (long)offset, SEEK_CUR ) == 0;
		}

		virtual bool SeekEnd (Bytes<usize> offset) override
		{
			return fseek( _file, (long)offset, SEEK_END ) == 0;
		}
		
		virtual Bytes<usize> RemainingSize () const override
		{
			usize size = 0;
			usize pos  = ftell( _file );

			fseek( _file, 0, SEEK_END );
			size = ftell( _file );
			fseek( _file, (long)pos, SEEK_SET );

			return Bytes<usize>( size - pos );
		}

		virtual Bytes<usize> Size () const override
		{
			usize size = 0;
			usize pos  = ftell( _file );

			fseek( _file, 0, SEEK_END );
			size = ftell( _file );
			fseek( _file, (long)pos, SEEK_SET );

			return Bytes<usize>( size );
		}

		virtual Bytes<usize> Pos () const override
		{
			return Bytes<usize>( ftell( _file ) );
		}
		
		virtual bool IsEOF () const override
		{
			return Pos() >= Size();
		}

		virtual StringCRef  Name () const override
		{
			return _name;
		}

		virtual EFile::type  GetType () const override
		{
			return EFile::HDD;
		}


	private:
		void _Close ()
		{
			if ( IsOpened() )
			{
				fclose( _file );
				_file = null;
			}
			_name.Clear();
		}
	};

}	// File


namespace GXTypes
{
	typedef SHARED_POINTER_TYPE( File::HddRFile )	HddRFilePtr;
	typedef SHARED_POINTER_TYPE( File::HddWFile )	HddWFilePtr;

}	// GXTypes
}	// GX_STL
