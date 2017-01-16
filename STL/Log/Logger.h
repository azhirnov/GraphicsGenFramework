// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	Logger -	log class for write html logs with color highlighting.
				message template: "date - time - [thread ID] - message"
*/

#pragma once

#include "../Files/HDDFile.h"
#include "../Defines/Errors.h"
#include "ELog.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Logger
	//

	class Logger
	{
	// variables
	private:
		WFilePtr		_logFile;
		Mutex			_lockLog;
		String			_crashFileName;
		String			_buffer;
		String			_projectFolder;		// project folder name for cuting full file path to short internal path

		// cache
		String			_lastSrcFile;
		int				_lastLine;
		ELog::type		_lastType;
		bool			_cached;


	// methods
	public:
		Logger ();
		~Logger ();

		void SetProjectFolderName (StringCRef folder);


		// Log File //
		bool Open (StringCRef filename, bool unique);
		void Close ();

		void Write (StringCRef msg, ELog::type type, StringCRef file, int line);


		// Crash File //
		bool CreateCrashFile (StringCRef filename);	// on start application
		bool CloseCrashFile ();						// on stop application

		bool ReadCrashFile (StringCRef name, OUT bool &wasCrashed, OUT Date &time) const;


		// Utils //
		void SendAnEmail ();

		static Logger * GetInstance ();


	private:
		void _ClearCache ();
		bool _CmpWithCache (ELog::type type, StringCRef file, int line);

		static void _GetDate (INOUT String &str);
		static void _GetDateForFName (INOUT String &str);
		static void _AddColor (INOUT String &str, ELog::type type);
		static char _GetPrefix (ELog::type type);

		static StringCRef _ToShortPath (StringCRef path, StringCRef folder);
	};
	

}	// GXTypes
}	// GX_STL
