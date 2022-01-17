#ifndef			_ACUI_CHATTING_EDIT_H_
#define			_ACUI_CHATTING_EDIT_H_

#include "AcUIEdit.h"

enum	ChattingEditInputType
{
	CHAT_INPUT_NORMAL		= 0	,
	CHAT_INPUT_WHISPER			,
	CHAT_INPUT_PARTY			,
	CHAT_INPUT_GUILD			,
	CHAT_INPUT_SHOUT
};

enum	ChattingEditInputStatus
{
	CHAT_INPUT_MESSAGE		= 0	,
	CHAT_INPUT_WHISPER_ID	= 1,
};

#define	NUMBER_OF_CHAT_HISTORY	8
#define	NUMBER_OF_ID_HISTORY	4

struct	ChatHistory
{
	char			szBuffer[512];				// 512 size의 문자열만 기록 -_-a
	ChatHistory*	bef;
	ChatHistory*	next;
};

struct	WhisperIDHistory
{
	char				szBuffer[32];
	WhisperIDHistory*	bef;
	WhisperIDHistory*	next;
};

class AcUIChattingEdit : public AcUIEdit
{
public:
	AcUIChattingEdit();
	~AcUIChattingEdit();

public:
	virtual	VOID	OnClose();
	virtual BOOL	OnChar( CHAR * pChar		, UINT lParam );
	virtual	BOOL	OnKeyDown( RsKeyStatus *ks	);
	virtual BOOL	OnIdle(UINT32 ulClockCount);

	virtual	VOID	OnWindowRender();
	virtual VOID	OnEditActive();

	void	UpdateCurrentStatus();		// 현재 입력 상태를 체크하여 갱신한다.
	void	ClearStatus();				// 상태 초기화

// Datas
	DWORD						m_ulCurTick;	

	ChattingEditInputType		m_eInputType;
	ChattingEditInputStatus		m_eInputStatus;

	// 2005.2.18 gemani .. 채팅 메시지를 역순으로 기억하고 같은 메시지는 추가X하고 환형큐 갱신만 한다..(ID역시 마찬가지)
	ChatHistory*			m_listChat;
	ChatHistory*			m_pCurChat;
	INT32					m_iChatNum;
	INT32					m_iChatStartPos;					// 메시지가 시작되는 포지션

	WhisperIDHistory*		m_listWhisperID;
	WhisperIDHistory*		m_pCurWhisperID;
	INT32					m_iWhisperIDNum;
	INT32					m_iWhisperIDStartPos;

	RwTexture*				m_pInputInfoTex;

	HKL						m_hKeyboardLayOut;

public :
	BOOL					IsDoubleByteCode_Kr( char* pString, int nIndex );
};

#endif			//_ACUI_CHATTING_EDIT_H_

