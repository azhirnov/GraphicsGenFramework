// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

namespace _libc_hidden_
{
	static inline
	char * _IntToA (char *buf, unsigned int val, int &size)
	{
		int	i = size - 2;

		if ( val == 0 )
		{
			buf[i--] = '0';
		}
		else
		{
			for(; val and i; --i, val /= 10)
			{
				buf[i] = "0123456789"[ val % 10 ];
			}
		}

		size = size - i - 2;
		return buf + i + 1;
	}

	static inline
	const char * _FormatAssertMessage (char *buf, int bufSize, const char *message, const char *file, int line)
	{
		int			pos			= 0;
		int			msgLength	= (int) ::strlen(message);
		const int	fileLength	= (int) ::strlen(file);
		const char	header0[]	= "Assertion failed!\n\nFile: ";
		const char	header1[]	= "\nLine: ";
		const char	header2[]	= "\n\nExpression:\n";
		const char	header3[]	= "\n\n(Press Retry to debug application)";
		char		lineStr[32]	= {0};
		int			lineLen		= sizeof(lineStr)-1;
		const char*	linePtr		= _IntToA( lineStr, line, lineLen );

		const int	totallen	= sizeof(header0)-1 + fileLength + sizeof(header1)-1 + lineLen +
								  sizeof(header2)-1 + msgLength + sizeof(header3)-1;

		if (totallen >= bufSize)
		{
			msgLength = bufSize - (totallen - msgLength) - 1;
			msgLength = msgLength < 0 ? 0 : msgLength;
		}

		memcpy( buf + pos, header0, sizeof(header0)-1 );	pos += sizeof(header0)-1;
		memcpy( buf + pos, file, fileLength );				pos += fileLength;
		memcpy( buf + pos, header1, sizeof(header1)-1 );	pos += sizeof(header1)-1;
		memcpy( buf + pos, linePtr, lineLen );				pos += lineLen;
		memcpy( buf + pos, header2, sizeof(header2)-1 );	pos += sizeof(header2)-1;
		memcpy( buf + pos, message, msgLength );			pos += msgLength;
		memcpy( buf + pos, header3, sizeof(header3)-1 );	pos += sizeof(header3)-1;

		buf[pos] = 0;
		return buf;
	}

	static inline
	const char * _FormatLogMessage (char *buf, int bufSize, const char *message, const char *file, int line)
	{
		int			pos			= 0;
		int			msgLength	= (int) ::strlen(message);
		const int	fileLength	= (int) ::strlen(file);
		const char	header0[]	= " (file: \"";
		const char	header1[]	= "\", line: ";
		const char	header2[]	= ")";
		char		lineStr[32]	= {0};
		int			lineLen		= sizeof(lineStr)-1;
		const char*	linePtr		= _IntToA( lineStr, line, lineLen );
		
		const int	totallen	= msgLength + sizeof(header0)-1 + fileLength + sizeof(header1)-1 +
								  lineLen + sizeof(header2)-1;
		
		if (totallen >= bufSize)
		{
			msgLength = bufSize - (totallen - msgLength) - 1;
			msgLength = msgLength < 0 ? 0 : msgLength;
		}

		memcpy( buf + pos, message, msgLength );			pos += msgLength;
		memcpy( buf + pos, header0, sizeof(header0)-1 );	pos += sizeof(header0)-1;
		memcpy( buf + pos, file, fileLength );				pos += fileLength;
		memcpy( buf + pos, header1, sizeof(header1)-1 );	pos += sizeof(header1)-1;
		memcpy( buf + pos, linePtr, lineLen );				pos += lineLen;
		memcpy( buf + pos, header2, sizeof(header2)-1 );	pos += sizeof(header2)-1;

		buf[pos] = 0;
		return buf;
	}

	static inline
	const char * _FormatConsoleMessage (char *buf, int bufSize, const char *message, const char *file, int line)
	{
		int			pos			= 0;
		int			msgLength	= (int) ::strlen(message);
		const int	fileLength	= (int) ::strlen(file);
		char		lineStr[32]	= {0};
		int			lineLen		= sizeof(lineStr)-1;
		const char*	linePtr		= _IntToA( lineStr, line, lineLen );
		
		const int	totallen	= fileLength + lineLen + msgLength + 5;
		
		if (totallen >= bufSize)
		{
			msgLength = bufSize - (totallen - msgLength) - 1;
			msgLength = msgLength < 0 ? 0 : msgLength;
		}

		memcpy( buf + pos, file, fileLength );				pos += fileLength;
		buf[pos++] = '(';
		memcpy( buf + pos, linePtr, lineLen );				pos += lineLen;
		buf[pos++] = ')';	buf[pos++] = ':';	buf[pos++] = ' ';
		memcpy( buf + pos, message, msgLength );			pos += msgLength;
		buf[pos++] = '\n';
		
		buf[pos] = 0;
		return buf;
	}

	enum {
		LIBC_MESSAGE_BUFFER_SIZE	= 1 << 12
	};

}	// _libc_hidden_


// Windows
#ifdef PLATFORM_WINDOWS

namespace _libc_hidden_
{
extern "C"
{
	enum {
		WIN_MB_ABORTRETRYIGNORE			= 0x00000002L,
		WIN_MB_ICONHAND					= 0x00000010L,
		WIN_MB_ICONERROR				= WIN_MB_ICONHAND,
		WIN_MB_SETFOREGROUND			= 0x00010000L,
		WIN_MB_TOPMOST					= 0x00040000L,
		WIN_IDABORT						= 3,
		WIN_IDRETRY						= 4,
		WIN_IDIGNORE					= 5,
	};
		
#	define WINAPI				__stdcall
#	define WINBASEAPI			GX_DLL_IMPORT
#	define DECLARE_HANDLE(name)	struct name##__{int unused;}; typedef struct name##__ *name
//#	define __in_opt
//#	define __in

	DECLARE_HANDLE( HWND );
		
	typedef const char *	LPCSTR;
	typedef const wchar_t *	LPCWSTR;
	typedef unsigned int	UINT;


	WINBASEAPI
	int
	WINAPI
	MessageBoxA(
		__in_opt HWND hWnd,
		__in_opt LPCSTR lpText,
		__in_opt LPCSTR lpCaption,
		__in UINT uType);

	WINBASEAPI
	int
	WINAPI
	MessageBoxW(
		__in_opt HWND hWnd,
		__in_opt LPCWSTR lpText,
		__in_opt LPCWSTR lpCaption,
		__in UINT uType);
	
	WINBASEAPI
	void
	WINAPI
	OutputDebugStringA(
		__in_opt LPCSTR lpOutputString
		);

	WINBASEAPI
	void
	WINAPI
	OutputDebugStringW(
		__in_opt LPCWSTR lpOutputString
		);

	GX_NO_RETURN void __CDECL exit(int _Code);
		
#	undef  WINAPI
#	undef  WINBASEAPI
#	undef  DECLARE_HANDLE
#	undef  __in_opt
#	undef  __in
	
}	// extern "C"
}	// _libc_hidden_


#define __DebugAssertion( _expr_, _file_, _line_, _message_, _caption_ ) \
		\
		if ( not (_expr_) ) \
		{ \
			using namespace _hidden_; \
			\
			char	__buf[ _libc_hidden_::LIBC_MESSAGE_BUFFER_SIZE ]; \
			int		__result = _libc_hidden_::MessageBoxA( \
									0, \
									_libc_hidden_::_FormatAssertMessage( __buf, sizeof(__buf), (_message_), (_file_), (_line_) ), \
									_caption_, \
									(int)_libc_hidden_::WIN_MB_ABORTRETRYIGNORE | \
									(int)_libc_hidden_::WIN_MB_ICONERROR | \
									(int)_libc_hidden_::WIN_MB_SETFOREGROUND | \
									(int)_libc_hidden_::WIN_MB_TOPMOST ); \
			\
			CONSOLE_OUTPUT( __buf ); \
			\
			switch ( __result ) \
			{ \
				case _libc_hidden_::WIN_IDRETRY :	GX_BREAK_POINT();			break; \
				case _libc_hidden_::WIN_IDABORT :	_libc_hidden_::exit(0);	break; \
			}; \
		}

#define __DebugConsoleWrite( _file_, _line_, _message_ ) \
		{ \
			using namespace _hidden_; \
			\
			char	__buf[ _libc_hidden_::LIBC_MESSAGE_BUFFER_SIZE ]; \
			_libc_hidden_::OutputDebugStringA( \
				_libc_hidden_::_FormatConsoleMessage( __buf, sizeof(__buf), _message_, _file_, _line_ ) ); \
		}

#define __DebugConsoleWrite_Info( _file_, _line_, _message_ )		__DebugConsoleWrite( (_file_), (_line_), (_message_) )
#define __DebugConsoleWrite_Warning( _file_, _line_, _message_ )	__DebugConsoleWrite( (_file_), (_line_), (_message_) )
#define __DebugConsoleWrite_Error( _file_, _line_, _message_ )		__DebugConsoleWrite( (_file_), (_line_), (_message_) )

#endif	// PLATFORM_WINDOWS


// Android
#ifdef PLATFORM_ANDROID

namespace _libc_hidden_
{
	extern "C" int __android_log_write(int prio, const char *tag, const char *text);

	enum {
		ANDROID_LOG_VERBOSE	= 2,
		ANDROID_LOG_DEBUG	= 3,
		ANDROID_LOG_INFO	= 4,
		ANDROID_LOG_WARN	= 5,
		ANDROID_LOG_ERROR	= 6,
		ANDROID_LOG_ASSERT	= 7,
	};
}

#define __DebugAssertionTag( _expr_, _file_, _line_, _message_, _tag_, _type_ ) \
		\
		if ( not (_expr_) ) \
		{ \
			using namespace _hidden_; \
			\
			char	__buf[ _libc_hidden_::LIBC_MESSAGE_BUFFER_SIZE ]; \
			\
			_libc_hidden_::__android_log_write( _type_, \
												_tag_, \
												_libc_hidden_::_FormatLogMessage( __buf, sizeof(__buf), \
																	(_message_), (_file_), (_line_) ) ); \
		}

#define __DebugAssertion( _expr_, _file_, _line_, _message_, _caption_ ) \
		__DebugAssertionTag( (_expr_), (_file_), (_line_), (_message_), _caption_, _libc_hidden_::ANDROID_LOG_ASSERT )

#define __DebugConsoleWrite( _file_, _line_, _message_ ) \
		__DebugAssertionTag( false, (_file_), (_line_), (_message_), "Debug message", _libc_hidden_::ANDROID_LOG_DEBUG )

#define __DebugConsoleWrite_Info( _file_, _line_, _message_ ) \
		__DebugAssertionTag( false, (_file_), (_line_), (_message_), "Debug message", _libc_hidden_::ANDROID_LOG_INFO )

#define __DebugConsoleWrite_Warning( _file_, _line_, _message_ ) \
		__DebugAssertionTag( false, (_file_), (_line_), (_message_), "Debug message", _libc_hidden_::ANDROID_LOG_WARN )

#define __DebugConsoleWrite_Error( _file_, _line_, _message_ ) \
		__DebugAssertionTag( false, (_file_), (_line_), (_message_), "Debug message", _libc_hidden_::ANDROID_LOG_ERROR )

#endif	// PLATFORM_ANDROID



#if not defined( __DebugAssertion )

#	pragma warning ("Used dummy realization of __DebugAssertion")
#	define __DebugAssertion( _expr_, _file_, _line_, _message_, _caption_ )

#endif


#if not defined( __DebugConsoleWrite )

#	pragma warning ("Used dummy realization of __DebugConsoleWrite")
#	define __DebugConsoleWrite( _file_, _line_, _message_ )

#endif


#define assert3( _expr_, _msg_, _cap_ )	__DebugAssertion( (_expr_), __FILE__, __LINE__, (_msg_), (_cap_) )
#define assert2( _expr_, _msg_ )		assert3( (_expr_), (_msg_), "Debug assertion" )
#define assert( _expr_ )				assert2( (_expr_), TOSTRING(_expr_) )

#if defined(__GX_DEBUG__)
#	define DEBUG_CONSOLE( _msg_ )		__DebugConsoleWrite( __FILE__, __LINE__, (_msg_) )
#else
#	define DEBUG_CONSOLE( _msg_ )		
#endif
