#pragma once
#include "afxwin.h"
#include "ApBase.h"

#include <rwcore.h>
#include <rpworld.h>
#include <rplodatm.h>
#include <rtcharse.h>
#include <rttiff.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rphanim.h>
#include <rtanim.h>
#include <rpskin.h>
#include <rpusrdat.h>


// CSetFace 대화 상자입니다.

class CSetFace : public CDialog
{
	DECLARE_DYNCREATE(CSetFace)

public:
	BOOL		SetFaceAtomic(RpAtomic *pstAtomic);

	CSetFace(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetFace();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHARACTER_FACE };
	CComboBox	m_csFaceAtomic;

protected:
	RpAtomic*	m_pstFaceAtomic;
	RpClump*	m_pstClump;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeFaceAtomic();
	afx_msg void OnOk();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
};
