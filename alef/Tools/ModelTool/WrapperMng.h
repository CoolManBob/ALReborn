#pragma once

#include "AgcEngineChild.h"

class WrapperMng
{
public:
	WrapperMng( AgcEngineChild& cEngineChild );
	~WrapperMng();

	BOOL	GetCharacterTemplate( INT32 lID, AgpdCharacterTemplate* pPublic, AgcdCharacterTemplate* pClient );
	BOOL	GetItemTemplate( INT32 lID, AgpdItemTemplate* pPublic, AgcdItemTemplate* pClient );
	BOOL	GetObjectTemplate( INT32 lID, ApdObjectTemplate* pPublic, AgcdObjectTemplate* pClient );

protected:
	AgcEngineChild&		m_cEngineChild;
};