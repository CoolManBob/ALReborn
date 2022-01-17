#pragma once

#include "AgcmUIManager2.h"
#include <AuXmlParser.h>
#include <string>
#include <vector>
using namespace std;

#define		TooltipLineList			std::list< stTooltipLine >				
#define		TooltipLineListIter		std::list< stTooltipLine >::iterator


struct	stTooltipLine 
{
	string					strLine;
	DWORD					dwColor;
};


struct	stSystemGridTooltip 
{
	string					strSectionName;
	TooltipLineList			listTooltipLine;	
};


class AgcmUIMainSystemGridTooltip
{
private:
	typedef		std::vector< stSystemGridTooltip* >					SystemGridTooltipVec;
	typedef		std::vector< stSystemGridTooltip* >::iterator		SystemGridTooltipVecIter;

public:
	AgcmUIMainSystemGridTooltip						( VOID );
	~AgcmUIMainSystemGridTooltip					( VOID );

	BOOL					Initialize					( AgcmUIManager2*	pUIManager , BOOL bEncrypt = FALSE );
	VOID					Destroy						( VOID );

	BOOL					LoadSystemGridTooltipXML	( IN CONST string&	strFullPathName );

	BOOL					OpenSystemGridTooltip		( IN CONST string&	strSection );
	BOOL					CloseSystemGridTooltip		( VOID );


private:
	stTooltipLine*			_GetString					( IN CONST string&	strSection , INT nIndex );
	DWORD					_LineColor					( CONST string&	strLineColor );

	AuXmlDocument			m_XMLDocument;
	AuXmlNode*				m_XMLRootNode;

	AcUIToolTip				m_csSystemGridToolTip;		//	System Grid Tooltip Window	

	SystemGridTooltipVec	m_vecSysGridTooltip;		//	System Grid °ü¸® º¤ÅÍ

	AgcmUIManager2*			m_pcsAgcmUIManager2;
	
};
