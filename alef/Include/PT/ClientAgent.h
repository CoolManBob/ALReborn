#pragma once
#include "sdkconfig.h"
#include "clientinterface.h"

class PTSDK_STUFF CClientAgent :
	public CClientInterface
{
public:
	CClientAgent(void);
	~CClientAgent(void);

	virtual void QuestEKey		( CSessionPtr &p_session, const char *challengeNum);

	virtual void OperateResult	( CSessionPtr &p_session, int operation, int result);

	virtual void RecvDownMessage(CSDMessage& v_msg);

	CClientInterface * SetRealInterface( CClientInterface * p_interface);
protected:
	CClientInterface * m_pRealInterface;
};
