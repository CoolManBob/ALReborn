#pragma once

#define KEYBOARD_STATE_MAX		255

enum eSystemKeyState
{
	eKeyStateShift	= 0,
	eKeyStateCtrl	= 1,
	eKeyStateAlt	= 2,
	eKeyStateMax,
};

enum eKeyboardState
{
	eKeyboardNone	= 0,
	eKeyboardDown	= 1,
	eKeyboardPress	= 2,
	eKeyboardUp		= 3,
	eKeyboardMax,
};

//----------------------------- CKeyStateNode --------------------------
typedef BOOL (*CallbackKeyboard)(PVOID pThis);
class CKeyStateNode
{
public:
	CKeyStateNode( CallbackKeyboard func, PVOID pThis, BYTE byKey, eKeyboardState eState = eKeyboardDown, BOOL bShift = 0, BOOL bCtrl = 0, BOOL bAlt = 0 )
	{
		m_funcKeyboard				= func;
		m_pThis						= pThis;
		m_byKey						= byKey;
		m_eState					= eState;
		m_bSystem[eKeyStateShift]	= bShift;
		m_bSystem[eKeyStateCtrl]	= bCtrl;
		m_bSystem[eKeyStateAlt]		= bAlt;
	}

public:
	CallbackKeyboard	m_funcKeyboard;
	PVOID				m_pThis;
	BYTE				m_byKey;
	eKeyboardState		m_eState;
	BOOL				m_bSystem[eKeyStateMax];
};
typedef CKeyStateNode* LPKeyStateNode;

typedef std::list< LPKeyStateNode >		KeyStateList;
typedef KeyStateList::iterator			KeyStateListItr;

//----------------------------- CKeyboard --------------------------
class CKeyboardMng : public KeyStateList, public CSingleton< CKeyboardMng >
{
public:
	CKeyboardMng(void);
	~CKeyboardMng(void);

	//KeyStateList °ü·ÃFunction
	void	Insert( CallbackKeyboard func, PVOID pThis, BYTE byKey, eKeyboardState eState = eKeyboardDown, bool bShift = 0, bool bCtrl = 0, bool bAlt = 0 )	{
		push_back( new CKeyStateNode( func, pThis, byKey, eState, bShift, bCtrl, bAlt ) );
	}
	void	Clear()		{
		for( KeyStateListItr Itr = begin(); Itr != end(); ++Itr )	{
			delete (*Itr);
			(*Itr) = NULL;
		}
		clear();
	}

	void	UpdateKeyboard();

private:
	void	PreUpdate();
	void	PostUpdate();

	void	Execute( LPKeyStateNode pKeyStateNode );

	BOOL	IsShift();
	BOOL	IsCtrl();
	BOOL	IsAlt();

private:
	BOOL			m_bSystem[eKeyStateMax];
	eKeyboardState	m_eState[KEYBOARD_STATE_MAX];
};

