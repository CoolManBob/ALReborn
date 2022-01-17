#pragma once

#include "AgcmUIManager2.h"
#include "AcArchlordInfo.h"
#include "AgcmLogin.h"



class AgcmUILoginSelect
{
public:
	AgcmUILoginSelect		( VOID );
	~AgcmUILoginSelect		( VOID );

	BOOL					Initialize				( AgcmUIManager2*	pManager , AgcmLogin* pcmLogin );

	VOID					OpenLoginSelect			( VOID );		//	Login Server 선택 창을 오픈
	VOID					CloseLoginSelect		( VOID );		//	Login Server 선택 창을 닫는다


	BOOL					IsRetryLoginServer		( VOID );		//	재접속 가능한 IP가 남았는지 검사
	VOID					ReConnectLoginServer	( VOID );		//	로그인 서버 재접속을 시도한다

	stLoginGroup*			GetLoginGroup			( CONST std::string strGroupName );


	static BOOL				CBSelectLoginServer		( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );
	static BOOL				CBMouseOnLoginServer	( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );

	static BOOL				CBSelectExit			( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );

protected:
	BOOL					_LoginSelectWindowInit	( VOID );

	AgcmUIManager2*			m_pUIManager;
	AgcmLogin*				m_pcmLogin;

	AcUIToolTip				m_csLoginSelectUI;	//	Tooltip 대신 Button을 붙여서 사용한다

	AcArchlordInfo			m_ArchlordLoginInfo;

	std::string				m_strLastSelectGroup;	//	로그인서버 셀렉트창에서 선택한 그룹
};
