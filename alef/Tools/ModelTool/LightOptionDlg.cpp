// LightOptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelTool.h"
#include "LightOptionDlg.h"

//enum   RpLightType
//{ 
//  rpNALIGHTTYPE = 0,
//  rpLIGHTDIRECTIONAL,
//  rpLIGHTAMBIENT,
//  rpLIGHTPOINT = rpLIGHTPOSITIONINGSTART, 
//  rpLIGHTSPOT,
//  rpLIGHTSPOTSOFT,
//  rpLIGHTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT 
//};

// CLightOptionDlg 대화 상자입니다.

#define POINT_LIGHT_RADIUS_FACTOR (0.05f)		//Point light properties...
#define POINT_LIGHT_NUM_VERTICES (50)

#define DIRECT_LIGHT_CYLINDER_LENGTH (5.0f)		//Direct light cylinder properties...
#define DIRECT_LIGHT_CYLINDER_DIAMETER (1.5f)     
#define DIRECT_LIGHT_NUM_VERTICES (20)

#define DIRECT_LIGHT_CONE_SIZE (3.0f)			//Direct light cone properties...
#define DIRECT_LIGHT_CONE_ANGLE (45.0f)    
#define SPOT_LIGHTS_RADIUS_FACTOR (0.05f)		//Spot light properties...
#define CONE_NUM_VERTICES (10)					//Effects both spot lights & the direct light... 

RwReal s_fLightRadius = 100.0f;
RwReal s_fLightConeAngle = 45.0f;
RwRGBA s_cLightColor = { 255, 255, 0, 255 };

IMPLEMENT_DYNAMIC(CLightOptionDlg, CDialog)

CLightOptionDlg::CLightOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightOptionDlg::IDD, pParent)
	, m_strType(_T(""))
	, m_fRadius(0)
	, m_fAngle(0)
{
	
}

CLightOptionDlg::~CLightOptionDlg()
{
}

void CLightOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LIGHT_TYPE, m_strType);
	DDX_Text(pDX, IDC_LIGHT_RADIUS, m_fRadius);
	DDX_Text(pDX, IDC_LIGHT_ANGLE, m_fAngle);
	DDX_Control(pDX, IDC_LIGHT_LIST, m_cbLightList);
}

BOOL CLightOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ASSERT( CModelToolApp::GetInstance() );
	UpdateControl();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BEGIN_MESSAGE_MAP(CLightOptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLightOptionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLightOptionDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_LIGHT_COLOR_OPEN, &CLightOptionDlg::OnBnClickedLightColorOpen)
	ON_CBN_SELCHANGE(IDC_LIGHT_LIST, &CLightOptionDlg::OnCbnSelchangeLightList)
END_MESSAGE_MAP()

// CLightOptionDlg 메시지 처리기입니다.

void CLightOptionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetControl( GetCurrLight() );

	OnOK();
}

void CLightOptionDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void CLightOptionDlg::OnBnClickedLightColorOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRpLight* pLight = GetCurrLight();
	if( !pLight )		return;

	CColorDialog	cDlg;
	//cDlg.m_cc.lpCustColors	= m_CustomColors;
	cDlg.m_cc.Flags			|= CC_RGBINIT | CC_FULLOPEN;
	cDlg.m_cc.rgbResult		= RGB( (pLight->GetColor()->red*255.f), (pLight->GetColor()->green*255.f), (pLight->GetColor()->blue*255.f) );

	if( IDOK == cDlg.DoModal()  )
	{
		m_cColor = cDlg.GetColor();
	}
}

void CLightOptionDlg::OnCbnSelchangeLightList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetControl( GetCurrLight() );
}

void CLightOptionDlg::UpdateControl()
{
	CRenderWare* pRenderWare = CModelToolApp::GetInstance()->GetRenderWare();
	if( !pRenderWare )			return;
	
	RpLightMap& mapRpLight = pRenderWare->GetRpLightMap();
	if( mapRpLight.empty() )	return;

	m_cbLightList.ResetContent();

	for( RpLightMapItr Itr = mapRpLight.begin(); Itr != mapRpLight.end(); ++Itr )
		m_cbLightList.AddString( (*Itr).first.c_str() );
	
	SetControl( NULL );
}

void CLightOptionDlg::DrawLight()
{
	CRenderWare* pRenderWare = CModelToolApp::GetInstance()->GetRenderWare();
	if( !pRenderWare )			return;
	
	RpLightMap& mapRpLight = pRenderWare->GetRpLightMap();
	if( mapRpLight.empty() )	return;

	for( RpLightMapItr Itr = mapRpLight.begin(); Itr != mapRpLight.end(); ++Itr )
	{
		CRpLight* pLight = (*Itr).second;
		switch( pLight->GetType() )
		{
		case rpLIGHTDIRECTIONAL:
			DrawDirect( pLight );
			break;
		//case rpLIGHTAMBIENT:
		//	break;
		case rpLIGHTPOINT:
			DrawPoint( pLight );
			break;
		case rpLIGHTSPOT:
		case rpLIGHTSPOTSOFT:	
			DrawCone( pLight, s_fLightConeAngle, s_fLightRadius * SPOT_LIGHTS_RADIUS_FACTOR, 1.f );
			break;
		}
	}
}

const static char* szLightType[rpLIGHTSPOTSOFT+1] = {
	"Empty",
	"Directional",
	"Ambient",
	"Point",
	"Spot",
	"SpotSoft"
};

void CLightOptionDlg::SetControl( CRpLight* pLight )
{
	if( !pLight )
	{
		//m_strType = "";
		//GetDlgItem( IDC_LIGHT_RADIUS )->EnableWindow( FALSE );
		//GetDlgItem( IDC_LIGHT_ANGLE )->EnableWindow( FALSE );
		//UpdateData( FALSE );
		return;	
	}

	//일단 전부다 Disable하고.. 쓰는것만. Enable..하자. ^^
	GetDlgItem( IDC_LIGHT_RADIUS )->EnableWindow( FALSE );
	GetDlgItem( IDC_LIGHT_ANGLE )->EnableWindow( FALSE );
	
	switch( pLight->GetType() )
	{
	case rpLIGHTDIRECTIONAL:
		break;
	case rpLIGHTAMBIENT:
		GetDlgItem( IDC_LIGHT_RADIUS )->EnableWindow( TRUE );
		m_fRadius = pLight->GetRadius();
		break;
	case rpLIGHTPOINT:
		break;
	case rpLIGHTSPOT:
	case rpLIGHTSPOTSOFT:	
		GetDlgItem( IDC_LIGHT_RADIUS )->EnableWindow( TRUE );
		GetDlgItem( IDC_LIGHT_ANGLE )->EnableWindow( TRUE );
		m_fRadius	= pLight->GetRadius();
		m_fAngle	= pLight->GetConeAngle();
		break;
	}

	//항상 있는것들.. 처리..
	m_strType	= szLightType[pLight->GetType()];
	
	CStatic* pStatic = (CStatic*)GetDlgItem( IDC_STATIC_COLOR );
	if( pStatic )
	{
		m_cColor = RGB( (pLight->GetColor()->red*255.f), (pLight->GetColor()->green*255.f), (pLight->GetColor()->blue*255.f) );
		CClientDC dc( pStatic );
		CBrush br( m_cColor );
		CBrush* pOld = dc.SelectObject( &br );
		CRect rc;
		pStatic->GetClientRect( rc );
		dc.FillRect( rc, &br );
		dc.SelectObject( pOld );
	}

	UpdateData( FALSE );
}

void CLightOptionDlg::GetControl( CRpLight* pLight )
{
	if( !pLight )	return;

	UpdateData( TRUE );

	switch( pLight->GetType() )
	{
	case rpLIGHTDIRECTIONAL:
		break;
	case rpLIGHTAMBIENT:
		pLight->SetRadius( m_fRadius );
		break;
	case rpLIGHTPOINT:
		break;
	case rpLIGHTSPOT:
	case rpLIGHTSPOTSOFT:	
		pLight->SetRadius( m_fRadius );
		pLight->SetConeAngle( m_fAngle );
		break;
	}

	//위치..

	//색상
	RwRGBAReal cRwColor;
	cRwColor.red	= (float)GetRValue( m_cColor ) / 255.f;
	cRwColor.green	= (float)GetGValue( m_cColor ) / 255.f;
	cRwColor.blue	= (float)GetBValue( m_cColor ) / 255.f;
	cRwColor.alpha	= 1.f;
	pLight->SetColor( cRwColor );
}

CRpLight*	CLightOptionDlg::GetCurrLight()
{
	CRenderWare* pRenderWare = CModelToolApp::GetInstance()->GetRenderWare();
	if( !pRenderWare )			return NULL;
	
	RpLightMap& mapRpLight = pRenderWare->GetRpLightMap();
	if( mapRpLight.empty() )	return NULL;

	int nCurSel = m_cbLightList.GetCurSel();

	int nStart = 0;
	for( RpLightMapItr Itr = mapRpLight.begin(); Itr != mapRpLight.end(); ++Itr, ++nStart )
	{
		if( nStart == nCurSel )
			return (*Itr).second;
	}

	return NULL;
}

void		CLightOptionDlg::DrawDirect( CRpLight* pLight )
{
	if( !pLight )		return;

	/*
     * Draw cylinder behind cone -> to produce 3D arrow...
     */

	CRwFrame cFrame( pLight->GetFrame() );
	CRwMatrix matLTM( cFrame.GetLTM() );
	RwV3d* right = matLTM.GetRight();
    RwV3d* up    = matLTM.GetUp();
    RwV3d* at    = matLTM.GetAt();
    RwV3d* pos   = matLTM.GetPos();

    RwV3d point;
    RwReal cosValue, sinValue;
    RwV3d dRight, dUp, dAt;
    RwIm3DVertex cone[(DIRECT_LIGHT_NUM_VERTICES*2)+1];
    RwImVertexIndex indices[DIRECT_LIGHT_NUM_VERTICES*3];

    for(RwInt32 i=0; i<(DIRECT_LIGHT_NUM_VERTICES*2); i+=2)
    {
        cosValue = (RwReal)(RwCos(i/(DIRECT_LIGHT_NUM_VERTICES/2.0f) * rwPI));
        sinValue = (RwReal)(RwSin(i/(DIRECT_LIGHT_NUM_VERTICES/2.0f) * rwPI));

        RwV3dScale(&dUp, up, sinValue * DIRECT_LIGHT_CYLINDER_DIAMETER);
        RwV3dScale(&dRight, right, cosValue * DIRECT_LIGHT_CYLINDER_DIAMETER);
        RwV3dScale(&dAt, at, -(DIRECT_LIGHT_CONE_SIZE + 1.0f));

        /*
         * Cylinder base vertices...
         */
        point.x = pos->x + dAt.x + dUp.x + dRight.x;
        point.y = pos->y + dAt.y + dUp.y + dRight.y;
        point.z = pos->z + dAt.z + dUp.z + dRight.z;

        RwIm3DVertexSetPos(&cone[i], point.x, point.y, point.z);
                       
        /*
         *  Cylinder top vertices 
         */
        RwV3dScale(&dAt, at, -(DIRECT_LIGHT_CYLINDER_LENGTH + DIRECT_LIGHT_CONE_SIZE) );
        point.x = pos->x + dAt.x + dUp.x + dRight.x;
        point.y = pos->y + dAt.y + dUp.y + dRight.y;
        point.z = pos->z + dAt.z + dUp.z + dRight.z;
        
        RwIm3DVertexSetPos(&cone[i+1], point.x, point.y, point.z);
    }

    /*
     * Set color & alpha of all points...
     */
    for(RwInt32 i=0; i<(2*DIRECT_LIGHT_NUM_VERTICES)+1; i++)
    {
        RwIm3DVertexSetRGBA(&cone[i], s_cLightColor.red, s_cLightColor.green, s_cLightColor.blue, 128);
    }
    
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    if( RwIm3DTransform(cone, 2*DIRECT_LIGHT_NUM_VERTICES, NULL, 0) )
    {
		//Draw cylinder...
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        
		//Close cylinder...
        RwIm3DRenderTriangle((2*DIRECT_LIGHT_NUM_VERTICES)-2, (2*DIRECT_LIGHT_NUM_VERTICES)-1,0);
        RwIm3DRenderTriangle((2*DIRECT_LIGHT_NUM_VERTICES)-1,1,0);        
        
        RwIm3DEnd();
    }
    
    for(RwInt32 i=0; i<(DIRECT_LIGHT_NUM_VERTICES*2)+1; i++)
    {
        RwIm3DVertexSetRGBA( &cone[i], s_cLightColor.red, s_cLightColor.green, s_cLightColor.blue, 255 ); 
    }

    /*
     * Set cylinder base center point...
     */    
    RwV3dScale(&dAt, at, -(DIRECT_LIGHT_CYLINDER_LENGTH + DIRECT_LIGHT_CONE_SIZE));
    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;
    RwIm3DVertexSetPos(&cone[DIRECT_LIGHT_NUM_VERTICES*2], point.x, point.y, point.z);
    
    /* 
     * Set up vertex list...
     */
    for(RwInt32 i=0; i<DIRECT_LIGHT_NUM_VERTICES; i++)
    {
        indices[(i*3)]   = (RwImVertexIndex)DIRECT_LIGHT_NUM_VERTICES*2;
        indices[(i*3)+1] = (RwImVertexIndex)((i+1)*2) +1;    /* 3, 5, 7, 9, etc.*/
        indices[(i*3)+2] = (RwImVertexIndex)(i*2)+1;         /* 1, 3, 5, 7, etc.*/       
    }
    
    indices[(DIRECT_LIGHT_NUM_VERTICES*3)-2] = 1;

    /*
     * Draw base...
     */
    if( RwIm3DTransform(cone, (2*DIRECT_LIGHT_NUM_VERTICES)+1, NULL, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, indices, DIRECT_LIGHT_NUM_VERTICES*3);
        RwIm3DEnd();
    }  
     
    /*
     * Set cylinder top center point...
     */    
    RwV3dScale(&dAt, at, -(DIRECT_LIGHT_CONE_SIZE + 1.0f));
    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;
    RwIm3DVertexSetPos(&cone[DIRECT_LIGHT_NUM_VERTICES*2], point.x, point.y, point.z);

    /* 
     * Set up vertex list...
     */
    for(RwInt32 i=0; i<DIRECT_LIGHT_NUM_VERTICES; i++)
    {
        indices[(i*3)]   = (RwImVertexIndex)(DIRECT_LIGHT_NUM_VERTICES*2);
        indices[(i*3)+1] = (RwImVertexIndex)i*2;         /* 0, 2, 4, 6, etc.*/
        indices[(i*3)+2] = (RwImVertexIndex)(i+1)*2;     /* 2, 4, 6, 8, etc.*/               
    }
    indices[(DIRECT_LIGHT_NUM_VERTICES*3)-1] = 0;
    
    /*
     * Draw cylinder top...
     */
    if( RwIm3DTransform(cone, (2*DIRECT_LIGHT_NUM_VERTICES)+1, NULL, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, indices, DIRECT_LIGHT_NUM_VERTICES*3);
        RwIm3DEnd();
    }

    /*
     * Draw inverted cone to act as arrow head...
     */
    DrawCone(pLight, DIRECT_LIGHT_CONE_ANGLE, DIRECT_LIGHT_CONE_SIZE, -2.0f);
}

void		CLightOptionDlg::DrawCone( CRpLight* pLight, RwReal fAngle, RwReal fSize, RwReal fRatio  )
{
	if( !pLight )		return;

	  /*
     * Function to draw a cone.
     * The Cone Ratio parameter set the ratio between the cone height 
     * and width, if this is set to be negative the cone is drawn 
     * inverted - this means that the cone becomes narrower along the at 
     * vector(the direction of the light). Usually it is set to 1.0f which 
     * draws a cone getting wider along the at vector (the direction of 
     * the light).
     */

	CRwFrame cFrame( pLight->GetFrame() );
	CRwMatrix matLTM( cFrame.GetLTM() );
	RwV3d* right = matLTM.GetRight();
    RwV3d* up    = matLTM.GetUp();
    RwV3d* at    = matLTM.GetAt();
    RwV3d* pos   = matLTM.GetPos();

    RwV3d point;
    RwReal cosValue, sinValue, coneAngleD;
    RwV3d dRight, dUp, dAt;
    RwIm3DVertex cone[CONE_NUM_VERTICES+1];
    RwImVertexIndex indices[CONE_NUM_VERTICES*3];

    for(RwInt32 i=1; i<CONE_NUM_VERTICES+1; i++)
    {
        cosValue = (RwReal)(RwCos(i/(CONE_NUM_VERTICES/2.0f) * rwPI) * RwSin(fAngle / 180.0f * rwPI));
        sinValue = (RwReal)(RwSin(i/(CONE_NUM_VERTICES/2.0f) * rwPI) * RwSin(fAngle / 180.0f * rwPI));

        RwV3dScale(&dUp, up, sinValue * fSize);
        RwV3dScale(&dRight, right, cosValue * fSize);

        coneAngleD = (RwReal)RwCos(fAngle / 180.0f * rwPI);

        RwV3dScale(&dAt, at, coneAngleD * fSize * fRatio);

        point.x = pos->x + dAt.x + dUp.x + dRight.x;
        point.y = pos->y + dAt.y + dUp.y + dRight.y;
        point.z = pos->z + dAt.z + dUp.z + dRight.z;

        RwIm3DVertexSetPos(&cone[i], point.x, point.y, point.z);        
    }

    /* 
     * Set up vertex list...
     */
    for(RwInt32 i=0; i < CONE_NUM_VERTICES; i++)
    {
        indices[(i*3)]   = (RwImVertexIndex)0;
        indices[(i*3)+1] = (RwImVertexIndex)i+2;
        indices[(i*3)+2] = (RwImVertexIndex)i+1;
    }
    
    indices[(CONE_NUM_VERTICES*3)-2] = 1;

    /*
     * Set color & alpha of all points...
     */
    for(RwInt32 i=0; i<(CONE_NUM_VERTICES+1); i++)
    {
        RwIm3DVertexSetRGBA(&cone[i], s_cLightColor.red, s_cLightColor.green, s_cLightColor.blue, 128);
    }
    
    /*
     * Set cone apex to light position...
     */
    RwIm3DVertexSetPos(&cone[0],  pos->x, pos->y, pos->z);

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    if( RwIm3DTransform(cone, (CONE_NUM_VERTICES+1), NULL, 0) )
    {
        /*
         * Draw inside of cone...
         */
        RwIm3DRenderPrimitive(rwPRIMTYPETRIFAN);
        RwIm3DRenderTriangle(0, CONE_NUM_VERTICES, 1);
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, indices, CONE_NUM_VERTICES*3);
        RwIm3DEnd();
    }

    /*
     * Change alpha of all points...
     */
    for(RwInt32 i=0; i<(CONE_NUM_VERTICES+1); i++)
    {
        RwIm3DVertexSetRGBA(&cone[i], s_cLightColor.red, s_cLightColor.green, s_cLightColor.blue, 255);
    }

     
    /*
     * Set cone apex to same level as cone base 
     */
    coneAngleD = (RwReal)RwCos(fAngle / 180.0f * rwPI);

    RwV3dScale(&dAt, at, coneAngleD * fSize * fRatio);

    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;

    RwIm3DVertexSetPos(&cone[0], point.x, point.y, point.z);

    /*
     * Draw base...
     */       
    if( RwIm3DTransform(cone, CONE_NUM_VERTICES+1, NULL, rwIM3D_ALLOPAQUE) )
    {
        if( fRatio > 0 )
        {
            RwIm3DRenderPrimitive(rwPRIMTYPETRIFAN);
            RwIm3DRenderTriangle(0, CONE_NUM_VERTICES, 1);
        }
        else
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRIFAN, indices, CONE_NUM_VERTICES*3);
        }

        RwIm3DEnd();
    }
       
    /*
     * Move cone apex by small offset...
     */
    RwV3dScale(&dAt, at, -0.05f);
    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;
    RwIm3DVertexSetPos(&cone[0], point.x, point.y, point.z);

    /*
     * Draw Lines...
     */    
    if( RwIm3DTransform(cone, CONE_NUM_VERTICES+1, NULL, rwIM3D_ALLOPAQUE) )
    {        
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, indices, CONE_NUM_VERTICES*3);
        RwIm3DEnd();
    }
}

void		CLightOptionDlg::DrawPoint( CRpLight* pLight )
{
	if( !pLight )		return;

	CRwFrame cFrame( pLight->GetFrame() );
	CRwMatrix matLTM( cFrame.GetLTM() );
	RwV3d* pos = matLTM.GetPos();

	RwIm3DVertex shape[POINT_LIGHT_NUM_VERTICES];
	for(RwInt32 i=0; i<POINT_LIGHT_NUM_VERTICES; i++)
	{
		RwV3d point;
		point.x = pos->x + (RwReal)RwCos(i/(POINT_LIGHT_NUM_VERTICES/2.0f) * rwPI) * s_fLightRadius * POINT_LIGHT_RADIUS_FACTOR;
		point.y = pos->y + (RwReal)RwSin(i/(POINT_LIGHT_NUM_VERTICES/2.0f) * rwPI) * s_fLightRadius * POINT_LIGHT_RADIUS_FACTOR;
		point.z = pos->z;

		RwIm3DVertexSetRGBA( &shape[i], s_cLightColor.red, s_cLightColor.green, s_cLightColor.blue, s_cLightColor.alpha );
		RwIm3DVertexSetPos( &shape[i], point.x, point.y, point.z );
	}

	if( RwIm3DTransform( shape, POINT_LIGHT_NUM_VERTICES, NULL,rwIM3D_ALLOPAQUE ) )
	{
		RwIm3DRenderPrimitive( rwPRIMTYPEPOLYLINE );
		RwIm3DRenderLine( POINT_LIGHT_NUM_VERTICES - 1, 0 );
		RwIm3DEnd();
	}
}