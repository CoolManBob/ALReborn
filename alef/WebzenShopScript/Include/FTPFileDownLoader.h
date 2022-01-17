
#pragma once

#include "include.h"

class CFTPFileDownLoader
{
public:
	CFTPFileDownLoader();
	virtual ~CFTPFileDownLoader();

public:
	WZResult DownLoadFiles(DownloaderType type, 
						   tstring strServerIP, 
						   unsigned short PortNum, 
						   tstring strUserName, 
						   tstring strPWD, 
						   tstring strRemotepath, 
						   tstring strlocalpath, 
						   bool bPassiveMode, 
						   CListVersionInfo Version, 
						   std::vector<tstring>	vScriptFiles);

	void	Break();

private:
	BOOL CreateFolder(tstring strFilePath);
	BOOL m_Break;
	FileDownloader*	m_pFileDownloader;
};
