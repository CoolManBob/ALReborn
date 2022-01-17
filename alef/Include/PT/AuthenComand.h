#pragma once
#include "sdkconfig.h"
#include "SessionInfo.h"

class CAuthenCommand
{
public:
	CSessionPtr m_Session;
	int			m_Command;
	vector<string> m_ParaArray;
};

