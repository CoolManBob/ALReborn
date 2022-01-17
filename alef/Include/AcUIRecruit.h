#ifndef		_ACUIRECRUIT_H_
#define		_ACUIRECRUIT_H_

#include "AcUIControlHeader.h"
#include "AcUIBbs.h"
//#include "AgcmRecruit.h"

#define	 ACUIRECRUIT_FIND_CLASS_NUM			4

// 크기 및 좌표 Define
#define	AGCUIRECRUIT_MAIN_WIDTH					499
#define	AGCUIRECRUIT_MAIN_HEIGHT				355
#define AGCUIRECRUIT_BBS_X						157
#define AGCUIRECRUIT_BBS_Y						36
#define	AGCUIRECRUIT_BBS_WIDTH					342
#define AGCUIRECRUIT_BBS_HEIGHT					314
#define	AGCUIRECRUIT_TAB1_X						257
#define	AGCUIRECRUIT_TAB1_Y						10
#define	AGCUIRECRUIT_TAB2_X						328
#define	AGCUIRECRUIT_TAB2_Y						10
#define	AGCUIRECRUIT_TAB_WIDTH					70
#define	AGCUIRECRUIT_TAB_HEIGHT					23
#define AGCUIRECRUIT_FIND_BUTTON_X				22
#define	AGCUIRECRUIT_FIND_BUTTON_Y				279
#define AGCUIRECRUIT_MY_LEVEL_FIND_BUTTON_X		22
#define	AGCUIRECRUIT_MY_LEVEL_FIND_BUTTON_Y		305
#define	AGCUIRECRUIT_BUTTON_WIDTH				116	
#define AGCUIRECRUIT_BUTTON_HEIGHT				23
#define	AGCUIRECRUIT_CLOSE_BUTTON_X				315	
#define AGCUIRECRUIT_CLOSE_BUTTON_Y				307	
#define AGCUIRECRUIT_CLOSE_BUTTON_WIDTH			16
#define AGCUIRECRUIT_CLOSE_BUTTON_HEIGHT		16
#define	AGCUIRECRUIT_SELECTBUTTON_START_X		43
#define	AGCUIRECRUIT_SELECTBUTTON_START_Y		73
#define	AGCUIRECRUIT_SELECTBUTTON_GAP_Y			20
#define	AGCUIRECRUIT_SELECTBUTTON_WIDTH			21
#define	AGCUIRECRUIT_SELECTBUTTON_HEIGHT		30
#define	AGCUIRECRUIT_EDITMINLEVEL_X				22	
#define	AGCUIRECRUIT_EDITMINLEVEL_Y				251
#define	AGCUIRECRUIT_EDITMAXLEVEL_X				79	
#define	AGCUIRECRUIT_EDITMAXLEVEL_Y				251
#define	AGCUIRECRUIT_EDIT_WIDTH					53	
#define	AGCUIRECRUIT_EDIT_HEIGHT				23

enum 
{
	ACUIRECRUIT_FIND_CLASS_KNIGHT	= 0,
	ACUIRECRUIT_FIND_CLASS_RANGER	= 1,
	ACUIRECRUIT_FIND_CLASS_MONK		= 2,
	ACUIRECRUIT_FIND_CALSS_MAGE		= 3
};


// Enum Control ID 
enum
{
	ACUIRECRUIT_CTRID_BBS1,
	ACUIRECRUIT_CTRID_BBS2,
	ACUIRECRUIT_CTRID_CLOSE,
	ACUIRECRUIT_CTRID_FIND1,
	ACUIRECRUIT_CTRID_FIND2,
	ACUIRECRUIT_CTRID_TAB1,
	ACUIRECRUIT_CTRID_TAB2,
};


/**		AcUIRecruit
	반드시 SetModulePointer 후에 ChildControlInit 해주고 사용한다. 
																	**/

class AgcmRecruit;

class AcUIRecruit : public AcUIModalImage
{
public:
	AcUIRecruit();
	~AcUIRecruit();

	AcUIBbs				m_clBbsMercenary		;
	AcUIBbs				m_clBbsParty			;

public:
	AgcmRecruit*	m_pAgcmRecruit				;
	
	AcUIPushButton	m_clTabMercenary			;
	AcUIPushButton	m_clTabParty				;
	//AcUICheckBox	m_clClassCheckbox[ACUIRECRUIT_FIND_CLASS_NUM]	;		// Class별 Check Box
	AcUIPushButton	m_clClassSelect[ACUIRECRUIT_FIND_CLASS_NUM]		;		// Class별 찾기 Push 버튼 
	AcUIButton		m_clButtonFind				;							// 찾기 버튼
	AcUIButton		m_clButtonMyLevelFind		;							// My Level 찾기 버튼 
	AcUIButton		m_clButtonClose				;							// 닫기 버튼 
//	AcUIEditNumber	m_clMinLevel				;							
//	AcUIEditNumber	m_clMaxLevel				;							
		
	BOOL			m_bBBSMode					;							// Mode : 0 - Mercenary , 1 - Party
	UINT32			m_lSelectedClass								;		
	INT32			m_lMinLevel										;
	INT32			m_lMaxLevel										;
		
public:
	//void	SetModulePointer( AgcmRecruit* pcmRecruit, AgcmCharacter* pcmCharacter )	;		// Set Module - Init 
	void	SetBbsMode( BOOL	bMode	)					;		// Mode : 0 - Mercenary , 1 - Party
	void	GetClassSelectInfo()							;		// Check Box 로 부터 Select상황을 알아낸다
	BOOL	ChildControlInit()								;		// Chilc Control을 초기화 한다 - 위치 등등
	
	virtual BOOL OnCommand		( INT32	nID , PVOID pParam	);
	
//	void	SetNextEdit()									;
};

#endif		//_ACUIRECRUIT_H_