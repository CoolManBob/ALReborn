#include "AgsdAuthKey.h"

BOOL AgsaAuthKeyPool::Initialize(INT32 lMaxPool)
{
	return InitializeObject(sizeof(AgsdAuthKey *), lMaxPool);
}

INT32 AgsaAuthKeyPool::GetAuthKey()
{
	return m_csRandom.randInt(100000000) + 1;
}

AgsdAuthKey* AgsaAuthKeyPool::AddAuthKey(CHAR *pszAccountName, CHAR *pszWorldName, UINT32 ulAddTimeMSec)
{
	if (!pszAccountName || !pszAccountName[0] || !pszWorldName || !pszWorldName[0])
		return NULL;

	AgsdAuthKey	*pcsAgsdAuthKey	= new AgsdAuthKey;
	if (!pcsAgsdAuthKey)
		return NULL;

	BOOL	bResult	= TRUE;

	bResult &= pcsAgsdAuthKey->SetAccountName(pszAccountName);
	bResult &= pcsAgsdAuthKey->SetWorldName(pszWorldName);
	bResult &= pcsAgsdAuthKey->SetAuthKey(GetAuthKey());
	bResult &= pcsAgsdAuthKey->SetAddTimeMSec(ulAddTimeMSec);

	if (!bResult || !AddObject(&pcsAgsdAuthKey, pszAccountName))
	{
		delete pcsAgsdAuthKey;
		return NULL;
	}

	return pcsAgsdAuthKey;
};

BOOL AgsaAuthKeyPool::RemoveAuthKey(CHAR *pszAccountName)
{
	AgsdAuthKey	*pcsAgsdAuthKey	= GetAuthKey(pszAccountName);
	if (!pcsAgsdAuthKey)
		return FALSE;

	if (!RemoveObject(pszAccountName))
		return FALSE;

	delete pcsAgsdAuthKey;

	return TRUE;
}

AgsdAuthKey* AgsaAuthKeyPool::GetAuthKey(CHAR *pszAccountName)
{
	AgsdAuthKey	**ppcsAgsdAuthKey	= (AgsdAuthKey **) GetObject(pszAccountName);
	if (!ppcsAgsdAuthKey || !*ppcsAgsdAuthKey)
		return NULL;

	return *ppcsAgsdAuthKey;
}

BOOL AgsaAuthKeyPool::ProcessTimeout(UINT32 ulCurrentTimeMSec, UINT32 ulTimeoutMSec)
{
	INT32	lIndex = 0;
	AgsdAuthKey **ppcsAgsdAuthKey = (AgsdAuthKey **) GetObjectSequence(&lIndex);

	while (ppcsAgsdAuthKey && *ppcsAgsdAuthKey)
	{
		if ((*ppcsAgsdAuthKey)->GetAddTimeMSec() + ulTimeoutMSec < ulCurrentTimeMSec)
			RemoveAuthKey((*ppcsAgsdAuthKey)->GetAccountName());

		ppcsAgsdAuthKey = (AgsdAuthKey **) GetObjectSequence(&lIndex);
	}

	return TRUE;
}