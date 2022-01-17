#pragma once

#include "RenderWare.h"
#include "AgcEngineXT.h"
#include "AgcmExportResource.h"
#include <tchar.h>
#include <stdio.h>

//---------------------- CConsole ----------------------
class CConsole
{
public:
	CConsole();
	~CConsole();

	static BOOL WINAPI CtrlHandler( DWORD dwCtrlType )	{	return TRUE;	}

protected:
	BOOL		m_bMake;
	HANDLE		m_hConsole;
};

//---------------------- XportTool ----------------------
enum eXportType
{
	eXportCharacter,
	eXportItem,
	eXportObject,
	eXportSkill,
	eXportEffect,
	eXportUI,
	eXportWorld,
	eXportETC,
	eXportINI,
	//eXportDDS,
	//eXportTexture,
	eXportMax
};

typedef BOOL (*ExportFunc)();
struct ExportNode
{
	ExportNode( char* command, char* str, ExportFunc func = NULL) : bUse(FALSE)
	{
		strCommand	= command;
		strString	= str;
		funcExport	= func;
	}

	BOOL		bUse;
	string		strCommand;
	string		strString;
	ExportFunc	funcExport;
};
typedef vector< ExportNode >		ExportNodeVec;
typedef ExportNodeVec::iterator		ExportNodeVecItr;

class XportTool : public CConsole
{
public:
	static XportTool*	m_pThis;
	static XportTool*	GetInst();

	XportTool();
	~XportTool();

	BOOL	Init( HWND hWnd );
	void	Destory();
	void	PrintConsole( const char* pStr );
	void	PrintFormatConsole( char* pFormat, ... );
	void	PrintLog( char* pFormat, ... );
	BOOL	RecreateExportFolder();

	BOOL	exportCharacterTemplate();
	BOOL	exportItemTemplate();
	BOOL	exportObjectTemplate();
	BOOL	exportSkillTemplate();
	BOOL	exportEffectResource();
	BOOL	exportUIResource();
	BOOL	exportWorldResource();
	BOOL	exportEtcTexture();
	BOOL	exportINIResource();
	BOOL	exportDDS();

public:
	AgcmExportResource	m_cExportResource;
	RenderWare			m_cRenderWare;
	AgcEngineXT			m_cEngine;
	
	HWND				m_hHwnd;

	ExportNodeVec		m_vecExportNode;
	string				m_strExportFilename;
	TCHAR				m_szBinPath[MAX_PATH];
	TCHAR				m_szExportPath[MAX_PATH];
};