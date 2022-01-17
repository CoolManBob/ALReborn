#ifndef	_AGCSENDERWINDOW_H_
#define	_AGCSENDERWINDOW_H_

#include "AgcEngine.h"

class AgcmTargeting;

class AgcSenderWindow : public AgcWindow
{
public:
	AgcSenderWindow();
	~AgcSenderWindow();

	inline void	SetTargetingClass( AgcmTargeting* pClass ){ m_pcmTargeting = pClass; }

public:
	virtual	BOOL OnInit			(					);	
	//virtual	void OnClose		(					);

	virtual	BOOL OnKeyDown		( RsKeyStatus *ks	);
	//virtual	BOOL OnKeyUp		( RsKeyStatus *ks	);
	virtual	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	virtual	BOOL OnLButtonDblClk( RsMouseStatus *ms	);
	virtual	BOOL OnLButtonUp	( RsMouseStatus *ms	);
	virtual	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	virtual	BOOL OnRButtonDblClk( RsMouseStatus *ms	);
	//virtual	BOOL OnRButtonUp	( RsMouseStatus *ms	);
	virtual	BOOL OnMouseMove	( RsMouseStatus *ms	);
	//virtual	BOOL OnMouseWheel	( INT32	lDelta		);	

	AgcmTargeting*		m_pcmTargeting;
};

#endif	// _AGCSENDERWINDOW_H_