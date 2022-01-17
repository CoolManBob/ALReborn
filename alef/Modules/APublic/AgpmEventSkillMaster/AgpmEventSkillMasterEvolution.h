#ifndef _AGPMEVENTSKILLMASTEREVOLUTION_H
#define _AGPMEVENTSKILLMASTEREVOLUTION_H
#include "ApDefine.h"

class AgpmEventSkillMasterEvolution
{
	struct _SkillMasteryEvolution
	{
		INT32 lTID;
		INT32 lDegree;
		INT32 lPrevTID;

		_SkillMasteryEvolution()
			: lTID(0), lDegree(0), lPrevTID(0)
		{
		}

		bool operator==(INT32 a)
		{
			return (lTID == a);
		}
	};

	typedef vector<_SkillMasteryEvolution> SkillMasteryEvolution;

	SkillMasteryEvolution m_vcEvolution;

public:
	AgpmEventSkillMasterEvolution()
	{
	};

	virtual ~AgpmEventSkillMasterEvolution()
	{
	};

	BOOL Initialize()
	{
		_SkillMasteryEvolution pRoot;
		m_vcEvolution.insert(m_vcEvolution.end(), pRoot);

		return TRUE;
	};

	BOOL Insert(INT32 lTID, INT32 lPrevTID)
	{
		SkillMasteryEvolution::iterator It2 = find(m_vcEvolution.begin(), m_vcEvolution.end(), lPrevTID);
		if(It2 != m_vcEvolution.end())
		{
			_SkillMasteryEvolution pEvolutionElem;
			pEvolutionElem.lTID = lTID;
			pEvolutionElem.lDegree = It2->lDegree + 1;
			pEvolutionElem.lPrevTID = It2->lTID;

			m_vcEvolution.insert(m_vcEvolution.end(), pEvolutionElem);
		}

		return TRUE;
	};

	INT32 GetEvolutionDegree(INT32 lTID)
	{
		SkillMasteryEvolution::iterator It = find(m_vcEvolution.begin(), m_vcEvolution.end(), lTID);

		return (It != m_vcEvolution.end()) ? It->lDegree : 0 ;
	};

	BOOL CheckEnableEvolution(INT32 lCharacterTID, INT32 lEvolutionTID)
	{
		SkillMasteryEvolution::iterator CharacterIt = find(m_vcEvolution.begin(), m_vcEvolution.end(), lCharacterTID);
		SkillMasteryEvolution::iterator EvolutionIt = find(m_vcEvolution.begin(), m_vcEvolution.end(), lEvolutionTID);

		if(CharacterIt == m_vcEvolution.end() || EvolutionIt == m_vcEvolution.end())
			return FALSE;

		if(EvolutionIt->lPrevTID != CharacterIt->lTID)
			return FALSE;

		return TRUE;
	};

	INT32 GetBeforeEvolutionTID(INT32 lTID)
	{
		SkillMasteryEvolution::iterator CharacterIt = find(m_vcEvolution.begin(), m_vcEvolution.end(), lTID);

		return (CharacterIt != m_vcEvolution.end()) ? CharacterIt->lPrevTID : 0;
	};
};

#endif // _AGPMEVENTSKILLMASTEREVOLUTION_H