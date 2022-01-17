#include <afxinet.h>
#include <Wininet.h>

class CInternetLib 
{
public:
	bool ConnectToFTP( char *pstrAddr, char *pstrID, char *pstrPW, int lPort );
	bool GetFileFromFTP();

private:
	CInternetSession	m_cSession;
    CFtpConnection*		m_pcsFTPConnection;
};