// AgcmAdminDlgCharacter.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 15.

// Character Dialog Class
// Property Sheet Dialog
class __declspec(dllexport) AgcmAdminDlgCharacter : public AgcmAdminDlgBase
{
protected:
	AgcmAdminDlgCharacterData m_csCharacterData;
	AgcmAdminDlgCharacterItem m_csCharacterItem;
	AgcmAdminDlgCharacterSkill m_csCharacterSkill;
	AgcmAdminDlgCharacterSkillSub m_csCharacterSkillSub;
	AgcmAdminDlgCharacterParty m_csCharacterParty;

	stAgpdAdminSearch m_stLastSearch;	// 마지막 검색한 캐릭터 정보
	stAgpdAdminCharData m_stCharData;	// 모든 페이지에서 Base 정보로 쓰인다.

public:
	AgcmAdminDlgCharacter();
	virtual ~AgcmAdminDlgCharacter();

	// Get Member Class
	AgcmAdminDlgCharacterData* GetDataPage();
	AgcmAdminDlgCharacterItem* GetItemPage();
	AgcmAdminDlgCharacterSkill* GetSkillPage();
	AgcmAdminDlgCharacterSkillSub* GetSkillSubPage();
	AgcmAdminDlgCharacterParty* GetPartyPage();

	// Property Sheet 에 맞게 끔 Overriding 한다.
	virtual BOOL OpenDlg();

	// Callback Class Override
	virtual BOOL SetCBClass(PVOID pClass);

	BOOL SetLastSearch(stAgpdAdminSearch* pstLastSearch);
	stAgpdAdminSearch* GetLastSearch();
	BOOL IsSearchCharacter(stAgpdAdminCharData* pstCharData);
	BOOL IsSearchCharacter(INT32 lCID);
	BOOL IsSearchCharacter(CHAR* szName);

	BOOL SetCharData(stAgpdAdminCharData* pstCharData);
	BOOL SetCharDataSub(stAgpdAdminCharDataSub* pstCharDatSub);
	stAgpdAdminCharData* GetCharData();
	
	BOOL SetCharSkillPoint(CHAR* szCharName, INT32 lTotalSP, INT32 lRemainSP);

	BOOL ShowBasicInfo(HWND hDlg);	// 각 페이지의 BasicInfo 를 뿌려준다.
	
	BOOL ActivateSkillSubPage();	// Skill Sub Page 를 보여준다.

protected:
	BOOL InitPropertySheet();
};