// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/STL/Engine.STL.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;


static void TestGetAllFiles ()
{
	Array<String>	dirs;

	ASSERT( OS::FileSystem::GetAllDirsInPath( "..", dirs ) );

	ASSERT( not dirs.Empty() );
}


static void TestFileSearch ()
{
	ASSERT( OS::FileSystem::FindAndSetCurrentDir( "Tests/Engine/Compute", 5 ) );

	ASSERT( OS::FileSystem::IsFileExist( "Main.cpp" ) );
}


extern void Test_OS_FileSystem ()
{
	TestGetAllFiles();
	TestFileSearch();
}
