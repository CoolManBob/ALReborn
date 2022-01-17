#include "AgpaCharacterTemplate.h"
#include "ApBase.h"

/*****************************************************************
*   Function : AgpaCharacterTemplate()
*   Comment  : 생성자 
*                    
*   Date&Time : 2002-04-15, 오후 4:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaCharacterTemplate::AgpaCharacterTemplate()
{
	
}
/*****************************************************************
*   Function : ~AgpaCharacterTemplate()
*   Comment  : 소멸자 
*                    
*   Date&Time : 2002-04-15, 오후 4:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaCharacterTemplate::~AgpaCharacterTemplate()
{

}

/*****************************************************************
*   Function : AddCharacterTemplate( AgpdCharacterTemplate*, UINT32 )
*   Comment  : Add ChracterTemplate by TID
*   Date&Time : 2002-04-16, 오후 3:37
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacterTemplate*	AgpaCharacterTemplate::AddCharacterTemplate( AgpdCharacterTemplate* pCharacterT, INT32 lTID)
{
	if (!AddObject( (PVOID *) &pCharacterT, lTID ))
	{
		// 마고자 (2005-12-27 오후 6:14:30) : 
		// 어드민 에러체크.
		switch( GetLastError() )
		{
		case AEC_PARAMETER_ERROR		:	MD_SetErrorMessage( "AEC_PARAMETER_ERROR	\n" ); break;
		case AEC_KEY_ALEREADY_EXIST		:	MD_SetErrorMessage( "AEC_KEY_ALEREADY_EXIST	\n" ); break;
		case AEC_OBJECT_COUNT_LIMITED	:	MD_SetErrorMessage( "AEC_OBJECT_COUNT_LIMITED\n" ); break;
		case AEC_INSERT_FAILED			:	MD_SetErrorMessage( "AEC_INSERT_FAILED		\n" ); break;
		default:							MD_SetErrorMessage( "AEC_UNKNOWN_ERROR		\n" ); break;
		}

		#ifdef _DEBUG
		// DebugBreak();
		#endif

		return NULL;
	}

	return pCharacterT;
}

/*****************************************************************
*   Function : GetCharacterTemplate( AgpdCharacterTemplate*, UINT32)
*   Comment  : Get Character Template by TID
*   Date&Time : 2002-04-16, 오후 3:41
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacterTemplate*	AgpaCharacterTemplate::GetCharacterTemplate( INT32 lTID )
{
	AgpdCharacterTemplate **pvRetVal = (AgpdCharacterTemplate **) GetObject( lTID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : GetCharacterTemplate( AgpdCharacterTemplate*, UINT32)
*   Comment  : Get Character Template by szTName
*   Date&Time : 2002-04-16, 오후 3:41
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacterTemplate*	AgpaCharacterTemplate::GetCharacterTemplate( CHAR *szTName )
{
	AgpdCharacterTemplate **pvRetVal = (AgpdCharacterTemplate **) GetObject( szTName );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : RemoveCharacter(UINT32)
*   Comment  : Eliminate Character Template by TID
*   Date&Time : 2002-04-16, 오후 3:42
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpaCharacterTemplate::RemoveCharacterTemplate(INT32	lTID, CHAR *szTName)
{
	return RemoveObject( lTID, szTName );
}

BOOL	AgpaCharacterTemplate::AddTemplateStringKey(INT32 lTID, CHAR *szTName)
{
	return AddStringKey(lTID, szTName);
}

/*****************************************************************
*   Function : LoadTemplateFromIni
*   Comment  : Load Template Data From Ini File 
*   Date&Time : 2002-04-18, 오후 1:53
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpaCharacterTemplate::LoadTemplateFromIni()
{

#ifdef	_TEST_

/*	AuIniFile	m_csIniFile;
	
	//set ini file path 
	m_csIniFile.SetPath("./CharacterTemplate.ini");
	//read ini file data into memory
	m_csIniFile.ReadFile();

	UINT16	nNumKeys = m_csIniFile.GetNumKeys();
	UINT16	nType;

	UINT32	lTID = 1;

	AgpdCharacterTemplate	stCharacterT;

	std::string szBuffer;
	std::string szkeyName;

	//insert template data into ApAdmin
	for ( UINT16 i = 0 ; i < nNumKeys ; i++ )
	{
		szkeyName = m_csIniFile.GetKeyName(i);		// get key name

		//set AgpdSkillTemplate data
		stCharacterT.m_lTID = lTID;
		
		stCharacterT.m_stCharacterType.m_nRace = m_csIniFile.GetValueI(szkeyName, "Race");
		stCharacterT.m_stCharacterType.m_cGender = m_csIniFile.GetValueI(szkeyName, "Gender");
		stCharacterT.m_stCharacterType.m_cClass = m_csIniFile.GetValueI(szkeyName, "Class");

		stCharacterT.m_stCharacterAttribute.m_nCON = m_csIniFile.GetValueI(szkeyName, "Con");
		stCharacterT.m_stCharacterAttribute.m_nSTR = m_csIniFile.GetValueI(szkeyName, "Str");
		stCharacterT.m_stCharacterAttribute.m_nINT = m_csIniFile.GetValueI(szkeyName, "Int");
		stCharacterT.m_stCharacterAttribute.m_nDEX = m_csIniFile.GetValueI(szkeyName, "Dex");
		stCharacterT.m_stCharacterAttribute.m_nCHA = m_csIniFile.GetValueI(szkeyName, "Cha");
		stCharacterT.m_stCharacterAttribute.m_cLevel = m_csIniFile.GetValueI(szkeyName, "Level");

		szBuffer = m_csIniFile.GetValue(szkeyName, "Name");
		strcpy(stCharacterT.m_szTName, szBuffer.c_str());
				
		lTID++;

		// add template data
		BOOL aaaaa = AddCharacterTemplate(&stCharacterT, stCharacterT.m_lTID );

		// Zero Memory
		ZeroMemory(&stCharacterT, sizeof(stCharacterT));
			
	}		
*/
#endif

	return TRUE;

}