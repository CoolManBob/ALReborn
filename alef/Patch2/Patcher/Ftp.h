#include <windows.h>
#include <afxinet.h>
#include "AuPackingManager.h"

class CPatcherFTP
{
public:
	bool UploadFile( const char *pstrFTPURL, const char *pstrID, const char *pstrPassword, char *pstrPath, char *pstrJZPFile, int lPort, BOOL bPassive );

	bool MakeFolder( CFtpConnection	*pcsFTPConnection, char *pstrPath );
};