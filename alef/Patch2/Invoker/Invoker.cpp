#include <windows.h>
#include <stdio.h>

void GetTargetDir(char* buffer, int bufferSize)
{
	char  path[_MAX_PATH] = {0, };
	DWORD pathSize = _MAX_PATH;

	::GetModuleFileName(NULL, path, pathSize);

	char drive[_MAX_DRIVE] = {0, };
	char dir  [_MAX_DIR]   = {0, };
	char fname[_MAX_FNAME] = {0, };
	char ext  [_MAX_EXT]   = {0, };
	
	_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	_snprintf_s(buffer, bufferSize, _TRUNCATE, "%s%s", drive, dir);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	UNREFERENCED_PARAMETER(hInst);

	char path[_MAX_PATH] = {0, };
	GetTargetDir(path, _MAX_PATH);
	
	::SetCurrentDirectory(path);

	::ShellExecute(NULL, "open", "Archlord.exe", NULL, NULL, SW_SHOWNORMAL);

	return 0;
}
