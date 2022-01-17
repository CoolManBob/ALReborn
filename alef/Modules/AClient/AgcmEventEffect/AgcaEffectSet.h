#ifndef __AGCA_EFFECT_SET_H__
#define __AGCA_EFFECT_SET_H__

#include "AgcdEffCtrl.h"

class AES_EFFCTRL_SET_LIST
{
public:
	EFFCTRL_SET*			m_pstNode;
	AES_EFFCTRL_SET_LIST*	m_pcsNext;

	AES_EFFCTRL_SET_LIST()
	{
		m_pstNode	= NULL;
		m_pcsNext	= NULL;
	}
};

class List_AgcmEffectSetNode
{
public:
	EFFCTRL_SET				*m_pcsNode;
	List_AgcmEffectSetNode	*m_pcsNext;

	List_AgcmEffectSetNode()
	{
		m_pcsNode				= NULL;
		m_pcsNext				= NULL;
	}
};

class AgcdEffectSetNodeData
{
public:
	INT32					m_lKey;
	List_AgcmEffectSetNode	*m_pcsHead;
	AgcdEffectSetNodeData	*m_pcsNext;

	AgcdEffectSetNodeData()
	{
		m_lKey					= 0;
		m_pcsHead				= NULL;
		m_pcsNext				= NULL;
	}
};

class AgcaEffectSet
{
public:
	AgcaEffectSet() : m_pcsHead(NULL)		{		}
	virtual ~AgcaEffectSet()				{		}

public:
	BOOL	AddNode(INT32 lKey, EFFCTRL_SET *pcsNode);
	BOOL	RemoveAllNode();
	BOOL	RemoveNode(INT32 lKey);

	VOID	GetNode(INT32 lKey, EFFCTRL_SET **ppNode);
	BOOL	GetNode(INT32 lKey, AES_EFFCTRL_SET_LIST **ppstList);
	INT32	GetHeadKey();

protected:
	VOID	AddNode(AgcdEffectSetNodeData *pcsData, EFFCTRL_SET *pcsNode);
	VOID	RemoveNode(AgcdEffectSetNodeData *pstData);

protected:
	AgcdEffectSetNodeData	*m_pcsHead;
};

#endif // __AGCA_EFFECT_SET_H__