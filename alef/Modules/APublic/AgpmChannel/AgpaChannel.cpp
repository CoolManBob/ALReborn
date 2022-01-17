#include "AgpaChannel.h"

AgpaChannel::AgpaChannel()
{
}

AgpaChannel::~AgpaChannel()
{
}

BOOL AgpaChannel::Add(TCHAR *szChannelName, AgpdChannel* pChannel)
{
	if (!AddObject((PVOID)&pChannel, szChannelName))
		return FALSE;

	return TRUE;	
}

AgpdChannel* AgpaChannel::Get(TCHAR *pszChannelName)
{
	AgpdChannel **ppChannel = (AgpdChannel**)GetObject(pszChannelName);
	if (NULL == ppChannel)
		return NULL;

	return *ppChannel;
}

BOOL AgpaChannel::Remove(TCHAR *pszChannelName)
{
	return RemoveObject(pszChannelName);
}