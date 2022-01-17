// MenuChecker.cpp
// 2003.08.18 steeple

#include "stdafx.h"
#include "resource.h"
#include "MenuChecker.h"

void CMenuChecker::Init()
{
	m_pcsMenu = NULL;

	SetOpenRepository(TRUE);
	SetWorkingFolder(TRUE);
	SetAddFile(TRUE);
	SetAddFolder(TRUE);
	SetDelete(TRUE);

	SetGetLatestVersion(TRUE);
	SetCheckOut(TRUE);
	SetCheckIn(TRUE);
	SetUndoCheckOut(TRUE);
	SetRefresh(TRUE);
	SetShowHistory(TRUE);

	SetCreateRepository(TRUE);
	SetDeleteRepository(TRUE);
	SetUserManager(TRUE);
}

void CMenuChecker::SetMenu(CMenu* pcsMenu)
{
	m_pcsMenu = pcsMenu;
}

void CMenuChecker::ReloadAll()
{
	ReloadFileSection();
	ReloadResourceKeeperSection();
	ReloadAdminSection();
}

void CMenuChecker::ReloadFileSection()
{
	if(m_pcsMenu == NULL)
		return;

	m_pcsMenu->EnableMenuItem(ID_OPEN_REPOSITORY, GetOpenRepository() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_SET_WORKING_FOLDER, GetWorkingFolder() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_ADD_FILE, GetAddFile() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_ADD_FOLDER, GetAddFolder() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_FILE_DELETE, GetDelete() ? MF_ENABLED : MF_GRAYED);
}

void CMenuChecker::ReloadResourceKeeperSection()
{
	if(m_pcsMenu == NULL)
		return;

	m_pcsMenu->EnableMenuItem(ID_GET_LATEST_VERSION, GetLatestVersion() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_CHECK_OUT, GetCheckOut() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_CHECK_IN, GetCheckIn() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_UNDO_CHECK_OUT, GetUndoCheckOut() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_REFRESH, GetRefresh() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_SHOW_HISTORY, GetShowHistory() ? MF_ENABLED : MF_GRAYED);
}

void CMenuChecker::ReloadAdminSection()
{
	if(m_pcsMenu == NULL)
		return;

	m_pcsMenu->EnableMenuItem(ID_CREATE_REPORITORY, GetCreateRepository() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_DELETE_REPOSITORY, GetDeleteRepository() ? MF_ENABLED : MF_GRAYED);
	m_pcsMenu->EnableMenuItem(ID_USER_MANAGER, GetUserManager() ? MF_ENABLED : MF_GRAYED);
}

void CMenuChecker::SetAdmin(BOOL bFlag)
{
	SetCreateRepository(bFlag);
	SetDeleteRepository(bFlag);
	SetUserManager(bFlag);
}