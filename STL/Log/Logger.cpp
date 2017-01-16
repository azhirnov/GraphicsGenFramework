// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "Logger.h"
#include "../ThreadSafe/Singleton.h"
#include "../Types/StringParser.h"

namespace GX_STL
{

/*
=================================================
	__write_to_console
=================================================
*/
	inline static void __write_to_console (const char *msg, ELog::type type, const char *file, int line)
	{
		DEBUG_ONLY(
			switch ( type & ELog::_FlagsMask )
			{
				case ELog::Info :
					__DebugConsoleWrite_Info( file, line, msg );
					break;

				case ELog::Warning :
					__DebugConsoleWrite_Warning( file, line, msg );
					break;

				case ELog::Error :
				case ELog::Fatal :
					__DebugConsoleWrite_Error( file, line, msg );
					break;

				default :
					__DebugConsoleWrite( file, line, msg );
			}
		)
	}

/*
=================================================
	__write_to_log
=================================================
*/
	void __write_to_log (const char *msg, ELog::type type, const char *file, int line)
	{
		if ( GXTypes::Logger::GetInstance() == null ) {
			__write_to_console( msg, type, file, line );
			return;
		}

		GXTypes::Logger::GetInstance()->Write( msg, type, file, line );
	}


namespace GXTypes
{

/*
=================================================
	constructor
=================================================
*/
	Logger::Logger ()
	{
		_ClearCache();

		_buffer.Reserve( 256 );
	}
	
/*
=================================================
	destructor
=================================================
*/
	Logger::~Logger ()
	{
		ASSERT_EXT( _logFile.IsNull(), "Log file must be closed!" );
		ASSERT_EXT( _crashFileName.Empty(), "Crash file must be closed!" );

		Close();
		CloseCrashFile();
	}
	
/*
=================================================
	GetInstance
=================================================
*/
	Logger * Logger::GetInstance ()
	{
		return SingletonMultiThread::Instance< Logger >();
	}

/*
=================================================
	SetProjectFolderName
=================================================
*/
	void Logger::SetProjectFolderName (StringCRef folder)
	{
		_projectFolder = folder;
	}

/*
=================================================
	Open
=================================================
*/
	bool Logger::Open (StringCRef name, bool unique)
	{
		Close();


		String	log_name;

		log_name.Reserve( 256 );

		// create directory
		StringCRef	log_path = FileAddressUtils::GetPath( name );

		if ( not FileSystem::IsDirectoryExist( log_path ) )
		{
			CHECK_ERR( FileSystem::CreateDirectories( log_path ) );
		}

		if ( unique )
		{
			for (uint i = 0; i < 100; ++i)
			{
				log_name.Clear();

				log_name << name << "_(";

				_GetDateForFName( log_name );

				log_name << ')';
				
				if ( i != 0 )
					log_name << '_' << i;

				log_name << ".html";

				if ( not FileSystem::IsFileExist( log_name ) )
					break;
			}
		}
		else
		{
			log_name.Clear();
			log_name << name << ".html";
		}
		

		SCOPELOCK( _lockLog );
		
		CHECK_ERR( (_logFile = File::HddWFile::New( log_name )).IsNotNull() );

		static const char	header[] =	"<html> <head> <title> log </title> </head> <body BGCOLOR=\"#ffffff\">"
										"<p><PRE><font face=\"Lucida Console, Times New Roman\""
										"size=\"2\" color=\"#000000\">\n";

		_logFile->Write( StringCRef( header ) );
		_logFile->Flush();
		return true;
	}
	
/*
=================================================
	Close
=================================================
*/
	void Logger::Close ()
	{
		SCOPELOCK( _lockLog );

		if ( _logFile.IsNull() )
			return;
		
		if ( _cached )
			_logFile->Write( StringCRef( "</details>" ) );

		_logFile->Write( StringCRef( "</font></PRE> </p> </body> </html>\n" ) );

		_logFile->Close();
		_logFile = null;

		_ClearCache();
	}

/*
=================================================
	CreateCrashFile
=================================================
*/
	bool Logger::CreateCrashFile (StringCRef filename)
	{
		CloseCrashFile();
		
		SCOPELOCK( _lockLog );

		String	crash_file;

		crash_file.Reserve( 256 );
		crash_file = FileAddressUtils::GetPath( filename );

		if ( not FileSystem::IsDirectoryExist( crash_file ) )
		{
			CHECK_ERR( FileSystem::CreateDirectories( crash_file ) );
		}

		crash_file.Clear();
		crash_file << filename << ".crash";

		WFilePtr	file;
		CHECK_ERR( (file = File::HddWFile::New( crash_file )).IsNotNull() );

		String	msg;

		msg << "Started at ";	_GetDate( msg );
		msg << " - ";

		CHECK_ERR( file->Write( msg.cstr(), msg.LengthInBytes() ) );

		_crashFileName = RVREF( crash_file );
		return true;
	}
	
/*
=================================================
	CloseCrashFile
=================================================
*/
	bool Logger::CloseCrashFile ()
	{
		SCOPELOCK( _lockLog );

		if ( _crashFileName.Empty() )
			return true;
		
		WFilePtr	file;
		CHECK_ERR( (file = File::HddWFile::New( _crashFileName, File::HddWFile::EOpenFlags::Append )).IsNotNull() );

		String	msg;

		msg << "Closed at ";	_GetDate( msg );
		msg << "\n";

		CHECK_ERR( file->Write( msg.cstr(), msg.LengthInBytes() ) );

		_crashFileName.Clear();
		return true;
	}
	
/*
=================================================
	ReadCrashFile
=================================================
*/
	bool Logger::ReadCrashFile (StringCRef filename, OUT bool &wasCrashed, OUT Date &time) const
	{
		wasCrashed	= false;
		time		= Date();

		String	fname = filename;

		if ( FileAddressUtils::GetExtension( fname ).Empty() ) {
			fname << ".crash";
		}

		if ( not _crashFileName.Empty() ) {
			CHECK_ERR( _crashFileName != fname );
		}
		

		RFilePtr	file;

		if ( not (file = File::HddRFile::New( fname )).IsNotNull() )
			return false;

		// read file
		String	data;
		Bytes<usize>	size = file->Size();

		CHECK_ALLOC( data.Reserve( size + 1 ) );

		CHECK_ERR( file->Read( data.ptr(), size ) );
		data.SetLength( size );

		// is started and closed?
		usize	pos = 0;
		bool	start_found = false;

		while ( data.Find( "Started", pos, pos ) )	{ start_found = true; ++pos; }

		if ( start_found and
			 not data.Find( "Closed", pos, pos ) )
		{
			wasCrashed = true;
		}

		return true;
	}
	
/*
=================================================
	SendAnEmail
=================================================
*/
	void Logger::SendAnEmail ()
	{
		TODO( "SendEmail" );
		//ESS()->GetApplication()->GetPlatform()->SendEmail();
	}
	
/*
=================================================
	Write
=================================================
*/
	void Logger::Write (StringCRef msg, ELog::type type, StringCRef file, int line)
	{
		SCOPELOCK( _lockLog );
		
		if ( msg.Empty() )
			return;

		__write_to_console( msg.cstr(), type, file.cstr(), line );
		
		if ( _logFile.IsNull() )
			return;

		const bool	with_spoiler = EnumEq( type, ELog::SpoilerFlag );
		const bool	prev_cached	 = _cached;
		const bool	is_cached	 = _CmpWithCache( type, file, line );

		String &	str = _buffer;	str.Clear();

		// remove 'new line'
		if ( msg.Back() == '\n' )
			msg = msg.SubString( 0, msg.Length()-1 );

		if ( prev_cached and is_cached )
		{
			str << '\n' << msg;
		}
		else
		if ( is_cached )
		{
			// append to previous message
			_AddColor( str, type );

			str << "<details><summary>...</summary>" << msg;
		}
		else
		{
			if ( prev_cached )
				str << "</details>";

			// message type prefix
			_AddColor( str, type );
			str << _GetPrefix( type );

			// date and time
			_AddColor( str, ELog::_SrcFile );
			str << " - ";
			_GetDate( str );

			// current thread, source file and line number
			str << " - [" << String().FormatAlignedI( Thread::GetCurrentThreadId(), 8, '0', 16 ) << "] - ";
			str << "(file: " << _ToShortPath( file, _projectFolder ) << ", line: " << line << ")\n";
		
			_AddColor( str, type );

			if ( with_spoiler )
			{
				StringCRef	summary;
				usize		pos = 0;
				StringParser::ReadLineToEnd( msg, pos, OUT summary );

				if ( EnumEq( type, ELog::OpenSpoilerFlag ) or ELog::IsError( type ) )
					str << "<details open>";
				else
					str << "<details>";

				str << "<summary>" << summary << "</summary>" << msg.SubString(pos) << "</details>";
			}
			else
				str << msg << '\n';
		}

		_logFile->Write( StringCRef( str ) );
		_logFile->Flush();
	}
	
/*
=================================================
	_ClearCache
=================================================
*/
	void Logger::_ClearCache ()
	{
		_cached		= false;
		_lastType	= ELog::type(-1);
		_lastLine	= 0;
		_lastSrcFile.Clear();
	}
	
/*
=================================================
	_CmpWithCache
=================================================
*/
	bool Logger::_CmpWithCache (ELog::type type, StringCRef file, int line)
	{
		if ( _lastType == type						and
			 not EnumEq( type, ELog::SpoilerFlag )	and
			 not ELog::IsError( type )				and
			 _lastSrcFile == file					and
			 _lastLine == line )
		{
			_cached = true;
			return true;
		}
		_cached			= false;
		_lastType		= type;
		_lastLine		= line;
		_lastSrcFile	= file;
		return false;
	}

/*
=================================================
	_AddColor
=================================================
*/
	inline void Logger::_AddColor (INOUT String &str, ELog::type type)
	{
		enum EColor
		{
			BLUE		= 0x0000FF,
			NAVY		= 0x0000AB,
			VIOLET		= 0x8A2BE2,

			LIME		= 0x32CD32,
			GREEN		= 0x688E23,
			DARCK_GREEN	= 0x007F00,

			ORANGE		= 0xFF7F00,
			RED			= 0xFF0000,
			//RED			= 0xAA0000,
			DARCK_RED	= 0xA00000,

			SILVER		= 0x909090,
		};

		uint	color = 0;

		switch ( type & ELog::_FlagsMask )
		{
			case ELog::Debug	:	color = NAVY;			break;
			case ELog::Info		:	color = DARCK_GREEN;	break;
			case ELog::Warning	:	color = ORANGE;			break;
			case ELog::Error	:	color = RED;			break;
			case ELog::Fatal	:	color = DARCK_RED;		break;
			case ELog::_SrcFile	:	color = SILVER;			break;
		}
		
		str << "</font><font color=\"#" << String().FormatAlignedI( color, 6, '0', 16 ) << "\">";
	}
	
/*
=================================================
	_GetPrefix
=================================================
*/
	inline char Logger::_GetPrefix (ELog::type type)
	{
		switch ( type & ELog::_FlagsMask )
		{
			case ELog::Debug	:	return 'D';
			case ELog::Info		:	return 'I';
			case ELog::Warning	:	return 'W';
			case ELog::Error	:	return 'E';
			case ELog::Fatal	:	return 'F';
		}
		return ' ';
	}

/*
=================================================
	_GetDate
----
	add date to string, format: "yyyy/mm/dm - hh:mm:ss"
=================================================
*/
	inline void Logger::_GetDate (INOUT String &str)
	{
		str << Date().Now().ToString( "yyyy/mm/dm - hh:mi:ss" );
	}
	
/*
=================================================
	_GetDateForFName
----
	add date to string, format: "yyyy.mm.dm-hh.mm.ss"
=================================================
*/
	inline void Logger::_GetDateForFName (INOUT String &str)
	{
		str << Date().Now().ToString( "yyyy.mm.dm-hh.mi.ss" );
	}
	
/*
=================================================
	_ToShortPath
=================================================
*/
	inline StringCRef Logger::_ToShortPath (StringCRef path, StringCRef folder)
	{
		usize	pos;

		if ( path.Find( folder, OUT pos, 0 ) )
		{
			return path.SubString( pos + folder.Length() );
		}
		return FileAddressUtils::ToShortPath( path, 3 );
	}


}	// GXTypes
}	// GX_STL
