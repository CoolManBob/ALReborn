#include "stdafx.h"
#include "..\EffTool.h"
#include "dlg_newanimitem.h"
#include "dlg_texuvselect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlg_NewAnimItem::CDlg_NewAnimItem(AgcdEffRenderBase* pEffRenderBase, AgcdEffAnim* pEffAnim, CWnd* pParent /*=NULL*/) : CDialog(CDlg_NewAnimItem::IDD, pParent), 
 m_pEffRenderBase(pEffRenderBase), 
 m_pEffAnim(pEffAnim), 
 m_bMultiUV(FALSE)
{
	m_ulTime	= 0;
	m_strVal	= _T("");

	Eff2Ut_ZEROBLOCK(m_rgba);
	Eff2Ut_ZEROBLOCK(m_rcTuTv);
	Eff2Ut_ZEROBLOCK(m_multiUV);
	Eff2Ut_ZEROBLOCK(m_v3dPos);
	Eff2Ut_ZEROBLOCK(m_stRev);
	Eff2Ut_ZEROBLOCK(m_angle);
	Eff2Ut_ZEROBLOCK(m_v3dScale);
}

void CDlg_NewAnimItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_NewAnimItem)
	DDX_Text(pDX, IDC_EDIT_TIME, m_ulTime);
	DDX_Text(pDX, IDC_EDIT_VAL, m_strVal);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_NewAnimItem, CDialog)
	//{{AFX_MSG_MAP(CDlg_NewAnimItem)
	ON_BN_CLICKED(IDC_BTN_DLG, OnBtnDlg)
	ON_EN_CHANGE(IDC_EDIT_TIME, OnChangeEditTime)
	ON_EN_CHANGE(IDC_EDIT_VAL, OnChangeEditVal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlg_NewAnimItem::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	vSetInitText();
	vToString();
	
	return TRUE;
}

void CDlg_NewAnimItem::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(FALSE);

	if( !m_pEffAnim )
	{
		ErrToWnd( "!m_pEffAnim" );
		CDialog::OnOK();
		return;
	}

	switch( m_pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:
		{
			AgcdEffAnim_Colr*	pAnimColr = dynamic_cast<AgcdEffAnim_Colr*>(m_pEffAnim);
			for(int i=0; i<pAnimColr->m_stTblColr.bForTool_GetNum(); ++i)
			{
				if( m_ulTime == pAnimColr->m_stTblColr.m_stlvecTimeVal[i].m_dwTime )
				{
					ErrToWnd("해당 시간이 존재 합니다.");
					return;
				}
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_TUTV:
		{
			AgcdEffAnim_TuTv*	pAnimTutv = dynamic_cast<AgcdEffAnim_TuTv*>(m_pEffAnim);
			for(int i=0; i<pAnimTutv->m_stTblRect.bForTool_GetNum(); ++i)
			{
				if( m_ulTime == pAnimTutv->m_stTblRect.m_stlvecTimeVal[i].m_dwTime )
				{
					ErrToWnd("해당 시간이 존재 합니다.");
					return;
				}
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_LINEAR:
		{
			AgcdEffAnim_Linear*	pAnimLinear = dynamic_cast<AgcdEffAnim_Linear*>(m_pEffAnim);
			for(int i=0; i<pAnimLinear->m_stTblPos.bForTool_GetNum(); ++i)
			{
				if( m_ulTime == pAnimLinear->m_stTblPos.m_stlvecTimeVal[i].m_dwTime )
				{
					ErrToWnd("해당 시간이 존재 합니다.");
					return;
				}
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION	:
		{
			AgcdEffAnim_Rev*	pAnimRev = dynamic_cast<AgcdEffAnim_Rev*>(m_pEffAnim);
			for(int i=0; i<pAnimRev->m_stTblRev.bForTool_GetNum(); ++i)
			{
				if( m_ulTime == pAnimRev->m_stTblRev.m_stlvecTimeVal[i].m_dwTime )
				{
					ErrToWnd("해당 시간이 존재 합니다.");
					return;
				}
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_ROTATION	:
		{
			AgcdEffAnim_Rot*	pAnimRot = dynamic_cast<AgcdEffAnim_Rot*>(m_pEffAnim);
			for(int i=0; i<pAnimRot->m_stTblDeg.bForTool_GetNum(); ++i)
			{
				if( m_ulTime == pAnimRot->m_stTblDeg.m_stlvecTimeVal[i].m_dwTime )
				{
					ErrToWnd("해당 시간이 존재 합니다.");
					return;
				}
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_SCALE:
		{
			AgcdEffAnim_Scale*	pAnimScale = dynamic_cast<AgcdEffAnim_Scale*>(m_pEffAnim);
			for(int i=0; i<pAnimScale->m_stTblScale.bForTool_GetNum(); ++i)
			{
				if( m_ulTime == pAnimScale->m_stTblScale.m_stlvecTimeVal[i].m_dwTime )
				{
					ErrToWnd("해당 시간이 존재 합니다.");
					return;
				}
			}
		}
		break;
	default:
		ToWnd( _T("UNKNOWN") );
		return;
	}
	
	CDialog::OnOK();
}

extern VOID COLORREFToRwRGBA(RwRGBA* prgba, COLORREF clr);
extern COLORREF RwRGBAToCOLORREF(const RwRGBA* prgba);

void CDlg_NewAnimItem::OnBtnDlg() 
{
	// TODO: Add your control notification handler code here
	if( !m_pEffAnim )
	{
		ErrToWnd("!m_pEffAnim");
		return;
	}
	
	switch( m_pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:
		{
			COLORREF	clr = RwRGBAToCOLORREF(&m_rgba);
			CColorDialog	dlgcolr(clr,0,this);
			if( IDOK == dlgcolr.DoModal() )
			{
				COLORREFToRwRGBA(&m_rgba, dlgcolr.GetColor());
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_TUTV:
		{
			if( !m_pEffRenderBase )
			{
				ErrToWnd("!m_pEffRenderBase");
				return;
			}

			CDlg_TexUVSelect	dlgUV(m_pEffRenderBase->m_cEffTexInfo.bGetPtrTex(), this);
			if( IDOK == dlgUV.DoModal() )
			{
				if( dlgUV.m_bMultiInput )
				{
					m_bMultiUV		= TRUE;
					m_multiUV.tdst	= dlgUV.m_nTimeDistance;
					m_multiUV.bgn	= dlgUV.m_nBeginIndex;
					m_multiUV.end	= dlgUV.m_nEndIndex;
					m_multiUV.row	= dlgUV.m_nRow;
					m_multiUV.col	= dlgUV.m_nCol;
				}
				else
				{
					m_rcTuTv.m_fLeft	= dlgUV.m_fLeft;
					m_rcTuTv.m_fTop		= dlgUV.m_fTop;
					m_rcTuTv.m_fRight	= dlgUV.m_fRight;
					m_rcTuTv.m_fBottom	= dlgUV.m_fBottom;
				}
			}
		}
		break;

	default:
		return;
	}

	vToString();
}

void CDlg_NewAnimItem::OnChangeEditTime() 
{
	UpdateData(TRUE);

	vToString();	
}

void CDlg_NewAnimItem::OnChangeEditVal() 
{
	UpdateData(TRUE);

	vGetValue();	
}

void CDlg_NewAnimItem::vSetInitText()
{
	if( !m_pEffAnim )
	{
		ErrToWnd("!m_pEffAnim");
		return;
	}

	CHAR tempbuff[20] = "";
	CString	strEx, strExistTime;
	switch( m_pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:
		{
			strEx = "time;alpha;red;green;blue";
			AgcdEffAnim_Colr*	pAnimColr = dynamic_cast<AgcdEffAnim_Colr*>(m_pEffAnim);
			for(int i=0; i<pAnimColr->m_stTblColr.bForTool_GetNum(); ++i)
			{
				sprintf(tempbuff, "%d, ", pAnimColr->m_stTblColr.m_stlvecTimeVal[i].m_dwTime);
				strExistTime += tempbuff;						
			}
			GetDlgItem(IDC_BTN_DLG)->EnableWindow(TRUE);
			this->SetWindowText("color");
		}
		break;
	case AgcdEffAnim::E_EFFANIM_TUTV:
		{
			strEx = "time;left;top;right;bottom";
			AgcdEffAnim_TuTv*	pAnimTutv = dynamic_cast<AgcdEffAnim_TuTv*>(m_pEffAnim);
			for(int i=0; i<pAnimTutv->m_stTblRect.bForTool_GetNum(); ++i)
			{
				sprintf(tempbuff, "%d, ", pAnimTutv->m_stTblRect.m_stlvecTimeVal[i].m_dwTime);
				strExistTime += tempbuff;
			}
			GetDlgItem(IDC_BTN_DLG)->EnableWindow(TRUE);
			this->SetWindowText("uv");
		}
		break;
	case AgcdEffAnim::E_EFFANIM_MISSILE:
		{
		}
		break;
	case AgcdEffAnim::E_EFFANIM_LINEAR:
		{
			strEx = "time;x;y;z";
			AgcdEffAnim_Linear*	pAnimLinear = dynamic_cast<AgcdEffAnim_Linear*>(m_pEffAnim);
			for(int i=0; i<pAnimLinear->m_stTblPos.bForTool_GetNum(); ++i)
			{
				sprintf(tempbuff, "%d, ", pAnimLinear->m_stTblPos.m_stlvecTimeVal[i].m_dwTime);
				strExistTime += tempbuff;
			}
			this->SetWindowText("linear");
		}
		break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION:
		{
			strEx = "time;height;radius;bottom";
			AgcdEffAnim_Rev*	pAnimRev = dynamic_cast<AgcdEffAnim_Rev*>(m_pEffAnim);
			for(int i=0; i<pAnimRev->m_stTblRev.bForTool_GetNum(); ++i)
			{
				sprintf(tempbuff, "%d, ", pAnimRev->m_stTblRev.m_stlvecTimeVal[i].m_dwTime);
				strExistTime += tempbuff;
			}
		}
		break;
	case AgcdEffAnim::E_EFFANIM_ROTATION:
		{
			strEx = "time;angle";
			AgcdEffAnim_Rot*	pAnimRot = dynamic_cast<AgcdEffAnim_Rot*>(m_pEffAnim);
			for(int i=0; i<pAnimRot->m_stTblDeg.bForTool_GetNum(); ++i)
			{
				sprintf(tempbuff, "%d, ", pAnimRot->m_stTblDeg.m_stlvecTimeVal[i].m_dwTime);
				strExistTime += tempbuff;
			}
			this->SetWindowText("rotation");
		}
		break;
	case AgcdEffAnim::E_EFFANIM_SCALE:
		{
			strEx = "time;x;y;z";
			AgcdEffAnim_Scale*	pAnimScale = dynamic_cast<AgcdEffAnim_Scale*>(m_pEffAnim);
			for(int i=0; i<pAnimScale->m_stTblScale.bForTool_GetNum(); ++i)
			{
				sprintf(tempbuff, "%d, ", pAnimScale->m_stTblScale.m_stlvecTimeVal[i].m_dwTime);
				strExistTime += tempbuff;
			}
			this->SetWindowText("scale");
		}
		break;
	case AgcdEffAnim::E_EFFANIM_PARTICLEPOSSCALE:
		{
			//
		}
		break;
	default:
		ToWnd( _T("UNKNOWN") );
		return;
	}

	GetDlgItem(IDC_STATIC_GUID)->SetWindowText( strEx );
	GetDlgItem(IDC_STATIC_EXIST_TIME)->SetWindowText( strExistTime );
};

void CDlg_NewAnimItem::vGetValue()
{
	if( !m_pEffAnim )
	{
		ErrToWnd("!m_pEffAnim");
		return;
	}

	switch( m_pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR		: 
		{	
			CString strtime, stra, strr, strg, strb;
			AfxExtractSubString(strtime	, m_strVal, 0, _T(';'));
			AfxExtractSubString(stra	, m_strVal, 1, _T(';'));
			AfxExtractSubString(strr	, m_strVal, 2, _T(';'));
			AfxExtractSubString(strg	, m_strVal, 3, _T(';'));
			AfxExtractSubString(strb	, m_strVal, 4, _T(';'));

			m_ulTime = atol(strtime);
			RwRGBA	rgba	= { 
				  static_cast<UINT8>(atoi(strr))
				, static_cast<UINT8>(atoi(strg))
				, static_cast<UINT8>(atoi(strb))
				, static_cast<UINT8>(atoi(stra))
			};
			m_rgba = rgba;
		}
		break;
	case AgcdEffAnim::E_EFFANIM_TUTV		: 
		{
			CString strtime, strl, strt, strr, strb;
			AfxExtractSubString(strtime	, m_strVal, 0, _T(';'));
			AfxExtractSubString(strl	, m_strVal, 1, _T(';'));
			AfxExtractSubString(strt	, m_strVal, 2, _T(';'));
			AfxExtractSubString(strr	, m_strVal, 3, _T(';'));
			AfxExtractSubString(strb	, m_strVal, 4, _T(';'));
			
			m_ulTime = atol(strtime);
			STUVRECT	uvRect( (float)atof(strl), (float)atof(strt), (float)atof(strr), (float)atof(strb) );
			m_rcTuTv = uvRect;
		}
		break;
	case AgcdEffAnim::E_EFFANIM_LINEAR		: 
		{
			CString strtime, strx, stry, strz;
			AfxExtractSubString(strtime	, m_strVal, 0, _T(';'));
			AfxExtractSubString(strx	, m_strVal, 1, _T(';'));
			AfxExtractSubString(stry	, m_strVal, 2, _T(';'));
			AfxExtractSubString(strz	, m_strVal, 3, _T(';'));
			
			m_ulTime = atol(strtime);
			RwV3d	pos = { (float)atof(strx), (float)atof(stry), (float)atof(strz) };
			m_v3dPos = pos;
		}
		break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION	: 
		{
			CString strtime, strh, strr, stra;
			AfxExtractSubString(strtime	, m_strVal, 0, _T(';'));
			AfxExtractSubString(strh	, m_strVal, 1, _T(';'));
			AfxExtractSubString(strr	, m_strVal, 2, _T(';'));
			AfxExtractSubString(stra	, m_strVal, 3, _T(';'));
			
			m_ulTime = atol(strtime);
			AgcdEffAnim_Rev::STREVOLUTION
					stRev( (float)atof(strh), (float)atof(strr), (float)atof(stra) );
			m_stRev = stRev;
		}
		break;
	case AgcdEffAnim::E_EFFANIM_ROTATION	: 
		{
			CString strtime, stra;
			AfxExtractSubString(strtime	, m_strVal, 0, _T(';'));
			AfxExtractSubString(stra	, m_strVal, 1, _T(';'));

			m_ulTime = atol(strtime);
			m_angle = (float)atof(stra);
		}
		break;
	case AgcdEffAnim::E_EFFANIM_SCALE		: 
		{
			CString strtime, strx, stry, strz;
			AfxExtractSubString(strtime	, m_strVal, 0, _T(';'));
			AfxExtractSubString(strx	, m_strVal, 1, _T(';'));
			AfxExtractSubString(stry	, m_strVal, 2, _T(';'));
			AfxExtractSubString(strz	, m_strVal, 3, _T(';'));
			
			m_ulTime = atol(strtime);
			RwV3d	scale = { (float)atof(strx), (float)atof(stry), (float)atof(strz) };
			m_v3dScale = scale;
		}
		break;
	}

	UpdateData(FALSE);
};

void CDlg_NewAnimItem::vToString()
{	
	if( !m_pEffAnim )
	{
		ErrToWnd( "!m_pEffAnim" );
		return ;
	}

	switch( m_pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR: 
		m_strVal.Format( "%u;%d;%d;%d;%d", m_ulTime, m_rgba.alpha, m_rgba.red, m_rgba.green, m_rgba.blue);
		break;
	case AgcdEffAnim::E_EFFANIM_TUTV: 
		m_strVal.Format("%u;%f;%f;%f;%f", m_ulTime, m_rcTuTv.m_fLeft, m_rcTuTv.m_fTop, m_rcTuTv.m_fRight, m_rcTuTv.m_fBottom);
		break;
	case AgcdEffAnim::E_EFFANIM_LINEAR: 
		m_strVal.Format( "%u;%f;%f;%f", m_ulTime, m_v3dPos.x, m_v3dPos.y, m_v3dPos.z );
		break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION: 
		m_strVal.Format( "%u;%f;%f;%f", m_ulTime, m_stRev.m_fHeight, m_stRev.m_fRadius, m_stRev.m_fAngle );
		break;
	case AgcdEffAnim::E_EFFANIM_ROTATION: 
		m_strVal.Format( "%u;%f", m_ulTime, m_angle );
		break;
	case AgcdEffAnim::E_EFFANIM_SCALE: 
		m_strVal.Format("%u;%f;%f;%f", m_ulTime, m_v3dScale.x, m_v3dScale.y, m_v3dScale.z );
		break;
	}

	UpdateData(FALSE);
};

LPVOID CDlg_NewAnimItem::bGetPtrVal()
{
	if( !m_pEffAnim )
	{
		ErrToWnd("!m_pEffAnim");
		return NULL;
	}

	switch( m_pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR		: return (LPVOID)&m_rgba;
	case AgcdEffAnim::E_EFFANIM_TUTV		: return (LPVOID)&m_rcTuTv;
	case AgcdEffAnim::E_EFFANIM_LINEAR		: return (LPVOID)&m_v3dPos;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION	: return (LPVOID)&m_stRev;
	case AgcdEffAnim::E_EFFANIM_ROTATION	: return (LPVOID)&m_angle;
	case AgcdEffAnim::E_EFFANIM_SCALE		: return (LPVOID)&m_v3dScale;
	}

	return NULL;
};