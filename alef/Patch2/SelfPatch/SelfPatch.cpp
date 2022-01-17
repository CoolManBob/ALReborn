// SelfPatch.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include ".\bindstatuscallback.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>


///////////////////////////////////////////////////////////////////////
//. 2006. 4. 27. Nonstopdj
//. desc.	simple self patching program.
//.			1. find archlord.exe and archlord.bak
//.			2. delete archlord.exe 
//.			3. rename archlord.bak and excute.
//.			4. if not find archlord.exe and archlord.bak 
//.			   then connect ftp site and download recently .exe file.

using namespace std;

#ifdef _KOR
//. global var
//string	g_strURL			= "http://download.archlord.com/archlord/archlord.exe";
string	g_strURL			= "http://patch-arch.krweb.nefficient.com/archlord.exe";
#elif defined _CHN
string	g_strURL			= "http://download.archlord.com/archlord/archlord.exe";
#elif defined _ENG
string	g_strURL			= "http://download.archlord.com/archlord/archlord.exe";
#endif

#define OPTIONARG	_T("-NONSTOPDJ")

const string g_strFilename		= "archlord.exe";
const string g_strBakFilename	= "archlord.bak";

BOOL	g_bDeleteFile		= FALSE;

void DownloadFile()
{
	//. download file.
	CBindStatusCallback callback(g_strURL, timeGetTime() + 60 * 1000);
	HRESULT hr = ::URLDownloadToFile(
    NULL, g_strURL.c_str(), g_strFilename.c_str(), 0, &callback);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if( argc == 2 && !_tcscmp(argv[1], OPTIONARG))
	{
		//. if not find archlord.exe and archlord.bak then connect ftp site and download recently .exe file.
		if(GetFileAttributes(g_strBakFilename.c_str()) == -1 && GetFileAttributes(g_strFilename.c_str()) == -1)
		{
			DownloadFile();
			ShellExecute(NULL, _T("open"), _T(g_strFilename.c_str()), NULL, NULL, SW_SHOW);
			return 0;
		}

		//. try delete "archlord.exe" file
		while(GetFileAttributes(g_strFilename.c_str()) != -1)
		{
			g_bDeleteFile = DeleteFile(g_strFilename.c_str());
		}

		//. if finded 'archlord.bak', rename the file name.
		if(GetFileAttributes(g_strBakFilename.c_str()) != -1 && g_bDeleteFile )
		{
			rename( g_strBakFilename.c_str(), g_strFilename.c_str() );
			ShellExecute(NULL, _T("open"), _T(g_strFilename.c_str()), NULL, NULL, SW_SHOW);
			return 0;
		}
		else
		{
			DownloadFile();
			ShellExecute(NULL, _T("open"), _T(g_strFilename.c_str()), NULL, NULL, SW_SHOW);
			return 0;
		}

		ShellExecute(NULL, _T("open"), _T(g_strFilename.c_str()), NULL, NULL, SW_SHOW);
	}
	return 0;
}

