// AgcmUISample.cpp: implementation of the AgcmUISample class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmUISample.h"
#include "AgpmCharacter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmUISample::AgcmUISample()
{
	SetModuleName("AgcmUISample");
	EnableIdle(TRUE);

	m_alValue[0]	= 0;
	m_alValue[1]	= 0;
	m_lMaxValue		= 10000;

	memset(m_alDepth, 0, sizeof(INT32) * 20);
	m_alDepth[1] = 1;
	m_alDepth[2] = 1;
	m_alDepth[3] = 2;
	m_alDepth[4] = 3;
	m_alDepth[5] = 2;
	m_alDepth[6] = 3;
	m_alDepth[7] = 4;
	m_alDepth[8] = 4;
}

AgcmUISample::~AgcmUISample()
{

}

BOOL			AgcmUISample::OnAddModule()
{
	m_pcsAgcmUIManager2 = (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	m_pcsAgpmGrid		= (AgpmGrid *) GetModule("AgpmGrid");

	if (!m_pcsAgcmUIManager2)
		return FALSE;

	m_pcsAgpmGrid->Init(m_astGrid + 0, 1, 1, 3);
	m_pcsAgpmGrid->Init(m_astGrid + 1, 1, 2, 3);

	m_pcsAgpmGrid->Init(m_astSkill + 0, 1, 1, 3);
	m_pcsAgpmGrid->Init(m_astSkill + 1, 1, 2, 3);
	m_pcsAgpmGrid->Init(m_astSkill + 2, 1, 3, 3);

	// User Data를 만들고 등록한다.
	{
		m_pstUserData = m_pcsAgcmUIManager2->AddUserData("Test_User_Data", m_alValue, sizeof(INT32), 20, AGCDUI_USERDATA_TYPE_INT32);
		if (!m_pstUserData)
			return FALSE;

		m_pstDepth = m_pcsAgcmUIManager2->AddUserData("Test_Depth", m_alDepth, sizeof(INT32), 20, AGCDUI_USERDATA_TYPE_INT32);
		if (!m_pstDepth)
			return FALSE;
		/*
		m_pstUserData = m_pcsAgcmUIManager2->CreateUserData();
		if (!m_pstUserData)
			return FALSE;

		m_pstUserData->m_szName = "Test_User_Data";
		m_pstUserData->m_stUserData.m_pvData = m_alValue; // 이 부분에 Data를 넣는다. (예를 들어 Character Pointer)
		m_pstUserData->m_stUserData.m_lCount = 2;
		m_pstUserData->m_stUserData.m_lDataSize = sizeof(INT32);

		if (m_pcsAgcmUIManager2->AddUserData(m_pstUserData) < 0)
			return FALSE;
		*/
	}

	// User Data를 만들고 등록한다.
	{
		m_pstUserData2 = m_pcsAgcmUIManager2->AddUserData("Test_User_Data1", &m_lMaxValue, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
		if (!m_pstUserData2)
			return FALSE;
		/*
		m_pstUserData2 = m_pcsAgcmUIManager2->CreateUserData();
		if (!m_pstUserData2)
			return FALSE;

		m_pstUserData2->m_szName = "Test_User_Data2";
		m_pstUserData2->m_stUserData.m_pvData = &m_lMaxValue; // 이 부분에 Data를 넣는다. (예를 들어 Character Pointer)
		m_pstUserData2->m_stUserData.m_lCount = 1;
		m_pstUserData2->m_stUserData.m_lDataSize = sizeof(INT32);

		if (m_pcsAgcmUIManager2->AddUserData(m_pstUserData2) < 0)
			return FALSE;
		*/
	}

	// Display를 만들고 등록한다.
	{
		if (!m_pcsAgcmUIManager2->AddDisplay(this, "Test_Display_Name", UIS_DID_NAME, CBDisplayCharacterInfo, AGCDUI_USERDATA_TYPE_INT32))
			return FALSE;
		/*
		AgcdUIDisplay *		pstDisplay;

		pstDisplay = m_pcsAgcmUIManager2->CreateDisplay();
		if (!pstDisplay)
			return FALSE;

		pstDisplay->m_szName = "Test_Display_Name";
		pstDisplay->m_lID = UIS_DID_NAME;
		pstDisplay->m_pvClass = this;
		pstDisplay->m_fnCallback = CBDisplayCharacterInfo;

		m_lDisplayID = m_pcsAgcmUIManager2->AddDisplay(pstDisplay);
		if (m_lDisplayID < 0)
			return FALSE;
		*/
	}

	// Function을 만들고 등록한다.
	{
		if (!m_pcsAgcmUIManager2->AddFunction(this, "Test_Function", CBTest, 1))
			return FALSE;
		/*
		AgcdUIFunction *	pstFunction;

		pstFunction = m_pcsAgcmUIManager2->CreateFunction();

		pstFunction->m_szName = "Test_Function";
		pstFunction->m_fnCallback = CBTest;
		pstFunction->m_lNumData = 1;
		pstFunction->m_aszData[0] = "Print할 Text를 선택하세요";
		pstFunction->m_pClass = this;

		if (m_pcsAgcmUIManager2->AddFunction(pstFunction) < 0)
			return FALSE;
		*/
	}

	// Event를 등록한다.
	{
		m_lEventID = m_pcsAgcmUIManager2->AddEvent("Test_Event");
		if (m_lEventID < 0)
			return FALSE;
	}

	// Grid를 등록한다.
	{
		m_pstGrid = m_pcsAgcmUIManager2->AddUserData("Test_Grid", m_astGrid, sizeof(AgpdGrid), 2, AGCDUI_USERDATA_TYPE_GRID);
		if (!m_pstGrid)
			return FALSE;
		/*
		if (!m_pcsAgcmUIManager2->AddGrid("Test_Grid", &m_stGrid))
			return FALSE;
		*/
	}

	// Skill Tree를 등록하한다.
	{
		m_pstSkill = m_pcsAgcmUIManager2->AddUserData("Test_Skill", m_astSkill, sizeof(AgpdGrid), 3, AGCDUI_USERDATA_TYPE_GRID);
		if (!m_pstSkill)
			return FALSE;
	}

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "ReturnTRUE", CBReturnTRUE, AGCDUI_USERDATA_TYPE_NONE))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "ReturnFALSE", CBReturnFALSE, AGCDUI_USERDATA_TYPE_NONE))
		return FALSE;

	m_pcsCustomBase = new AcUIBase;
	if (!m_pcsAgcmUIManager2->AddCustomControl("TestControl", m_pcsCustomBase))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUISample::OnInit()
{
	return TRUE;
}

BOOL			AgcmUISample::OnIdle(UINT32 ulClockCount)
{
	for (INT32 i = 0; i < 19; ++i)
	{
		m_alValue[i] += 10;
		m_alValue[i + 1] += m_alValue[i];
	}

	if (m_alValue[0] >= m_lMaxValue)
	{
		m_alValue[0] = 0;
		m_alValue[1] = 0;
	}

	if (!(m_alValue[0] % 40))
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUserData);

	return TRUE;
}

BOOL			AgcmUISample::OnDestroy()
{
	m_pcsAgpmGrid->Remove(m_astGrid + 0);
	m_pcsAgpmGrid->Remove(m_astGrid + 1);

	delete m_pcsCustomBase;

	return TRUE;
}

BOOL			AgcmUISample::CBTest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISample *	pThis = (AgcmUISample *) pClass;
	AgcdUIControl *	pcsControl = (AgcdUIControl *) pData1;

	if (pcsControl && pcsControl->m_lType == AcUIBase::TYPE_EDIT)
	{
		((AcUIEdit *) pcsControl->m_pcsBase)->SetText("하하하");
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventID);

	return TRUE;
}

BOOL			AgcmUISample::CBDisplayCharacterInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	switch (lID)
	{
	case UIS_DID_NAME:
		sprintf(szDisplay, "Test Number(%d) : %d", pData, *(INT32 *) pData);
		*plValue = *(INT32 *) pData;
		break;

	case UIS_DID_HP:
	case UIS_DID_MP:
	case UIS_DID_SP:
		sprintf(szDisplay, "30");
		break;
	}

	return TRUE;
}

BOOL		AgcmUISample::CBReturnTRUE(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	return TRUE;
}

BOOL		AgcmUISample::CBReturnFALSE(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	return FALSE;
}
