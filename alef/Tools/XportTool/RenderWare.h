#pragma once

#include <windows.h>

class RenderWare
{
public:
	RenderWare()			{			}
	virtual	~RenderWare()	{			}

	bool initialize(HWND hWnd);
	void destroy();

private:
	bool attachPlugins();
	bool installFileSystem();
	bool registerImageLoaders();
};