// XTPGI_PSys.cpp: implementation of the CXTPGI_PSys class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_PSys.h"

#include "AgcuEffPath.h"
#include "AgcdEffParticleSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//-------------------- CXTPGI_CofEnvrn ---------------------
CXTPGI_CofEnvrn::CXTPGI_CofEnvrn(CString strCaption, stCofEnvrnParam* pCofEnvrn)
	: CXTPPropertyGridItem(strCaption)
	, m_pCofEnvrn(pCofEnvrn) 
	, m_pItemCofGrav(NULL)
	, m_pItemCofAirResistance(NULL)
{
	ASSERT(m_pCofEnvrn);
};

void CXTPGI_CofEnvrn::OnAddChildItem()
{
	ASSERT(m_pCofEnvrn);
	m_pItemCofGrav = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("cof gravity"), &m_pCofEnvrn->m_fCofGrav, m_pCofEnvrn->m_fCofGrav));
	m_pItemCofAirResistance = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("cof air-resistance"), &m_pCofEnvrn->m_fCofAirResistance, m_pCofEnvrn->m_fCofAirResistance));
};

class CXTPGI_PSys::CXTPGI_Emiter : public CXTPPropertyGridItem
{
	class CXTPGI_PGBox : public CXTPPropertyGridItem
	{
	public:
		CXTPGI_PGBox(stPGroupBox* pBox)
			: CXTPPropertyGridItem(_T("particles group"), _T("box") )
			, m_pBox(pBox)
			, m_pItemWidth(NULL), m_pItemHeight(NULL), m_pItemDepth(NULL)
		{
			ASSERT(m_pBox);
			CXTPPropertyGridItem::SetReadOnly(TRUE);
		};
		virtual ~CXTPGI_PGBox(){};

	protected:
		virtual void OnAddChildItem()
		{
			ASSERT(m_pBox);
			m_pItemWidth = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("half width"), &m_pBox->m_fHWidth, m_pBox->m_fHWidth));
			m_pItemHeight = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("half height"), &m_pBox->m_fHHeight, m_pBox->m_fHHeight));
			m_pItemDepth = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("half depth"), &m_pBox->m_fHDepth, m_pBox->m_fHDepth));
		}

	private:
		stPGroupBox*	m_pBox;
		CXTPGI_Float*	m_pItemWidth;
		CXTPGI_Float*	m_pItemHeight;
		CXTPGI_Float*	m_pItemDepth;
	};

	class CXTPGI_PGCylinder : public CXTPPropertyGridItem
	{
	public:
		CXTPGI_PGCylinder(stPGroupCylinder* pCylinder)
			: CXTPPropertyGridItem(_T("particles group"), _T("cylinder") )
			, m_pCylinder(pCylinder)
			, m_pItemRaius(NULL), m_pItemHeight(NULL)
		{
			ASSERT(m_pCylinder);
			CXTPPropertyGridItem::SetReadOnly(TRUE);
		};
		virtual ~CXTPGI_PGCylinder(){};

	protected:
		virtual void OnAddChildItem()
		{
			ASSERT(m_pCylinder);
			m_pItemRaius = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("radius"), &m_pCylinder->m_fRadius, m_pCylinder->m_fRadius));
			m_pItemHeight = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("half height"), &m_pCylinder->m_fHHeight, m_pCylinder->m_fHHeight));
		}

	private:
		stPGroupCylinder*	m_pCylinder;
		CXTPGI_Float*		m_pItemRaius;
		CXTPGI_Float*		m_pItemHeight;
	};

	class CXTPGI_PGSphere : public CXTPPropertyGridItem
	{
	public:
		CXTPGI_PGSphere(stPGroupSphere* pSphere)
			: CXTPPropertyGridItem(_T("particles group"), _T("sphere") )
			, m_pSphere(pSphere)
			, m_pItemRaius(NULL)
		{
			ASSERT(m_pSphere);
			CXTPPropertyGridItem::SetReadOnly(TRUE);
		};
		virtual ~CXTPGI_PGSphere(){};

	protected:
		virtual void OnAddChildItem()
		{
			ASSERT(m_pSphere);
			m_pItemRaius = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("radius"), &m_pSphere->m_fRadius, m_pSphere->m_fRadius));
		}

	private:
		stPGroupSphere*	m_pSphere;
		CXTPGI_Float*	m_pItemRaius;
	};

	AgcdEffParticleSystem::EMITER*	m_pEmiter;

	CXTPGI_Float*			m_pItemPower;
	CXTPGI_Float*			m_pItemPowerOffSet;
	CXTPGI_Float*			m_pItemGunLength;
	CXTPGI_Float*			m_pItemGunLengthOffset;	
	CXTPGI_UINT*			m_pItemOneShootNum;
	CXTPGI_UINT*			m_pItemOneShootNumOffset;
	CXTPGI_Float*			m_pItemOmegaLX;	//omega : angular velocity, LX : local x-axis
	CXTPGI_Float*			m_pItemOmegaWY; //omega : angular velocity, WY : world y-axis
	CXTPGI_Float*			m_pItemYawMax;	//포 도리각 제한
	CXTPGI_Float*			m_pItemYawMin;	//포 도리각 제한
	CXTPGI_Float*			m_pItemPitchMax;//포 끄덕각 제한
	CXTPGI_Float*			m_pItemPitchMin;//포 끄덕각 제한
	CXTPGI_RwV3d*			m_pItemGunsDir;
	CXTPGI_RwV3d*			m_pItemGunsSide;
	CXTPGI_Float*			m_pItemConeAngle;

	CXTPGI_Enum<ePGroup>*	m_pItemPGroupType;

	union{
		CXTPGI_PGBox*		m_pItemPGroupBox;
		CXTPGI_PGCylinder*	m_pItemPGroupCylinder;
		CXTPGI_PGSphere*	m_pItemPGroupSphere;
	};

	//flags
	CXTPGI_FlagBool*		m_pItemBool_SHOOTRAND;
	CXTPGI_FlagBool*		m_pItemBool_SHOOTCONE;
	CXTPGI_FlagBool*		m_pItemBool_YAWONEDIR;
	CXTPGI_FlagBool*		m_pItemBool_YAWBIDIR;
	CXTPGI_FlagBool*		m_pItemBool_PITCHONEDIR;
	CXTPGI_FlagBool*		m_pItemBool_PITCHBIDIR;
	CXTPGI_FlagBool*		m_pItemBool_NODEPND	;
	CXTPGI_FlagBool*		m_pItemBool_SHOOTNOMOVE;

	public:
		CXTPGI_Emiter(AgcdEffParticleSystem::EMITER* pEmiter) : CXTPPropertyGridItem(_T("emiter"))
		 ,m_pEmiter(pEmiter)
		{
			ASSERT( m_pEmiter );
			CXTPPropertyGridItem::SetReadOnly(TRUE);
		};
		virtual ~CXTPGI_Emiter(){};

	protected:
		virtual void OnAddChildItem()
		{
			ASSERT( m_pEmiter );
			m_pItemPower = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("power"), &m_pEmiter->m_fPower, m_pEmiter->m_fPower));
			m_pItemPowerOffSet = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("power offset"), &m_pEmiter->m_fPowerOffset, m_pEmiter->m_fPowerOffset));
			m_pItemGunLength = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("gun length"), &m_pEmiter->m_fGunLength, m_pEmiter->m_fGunLength));
			m_pItemGunLengthOffset = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("gun length offset"), &m_pEmiter->m_fGunLength, m_pEmiter->m_fGunLength));
			m_pItemOneShootNum = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("one shoot num"), &m_pEmiter->m_nNumOfOneShoot, m_pEmiter->m_nNumOfOneShoot));
			m_pItemOneShootNum = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("one shoot num offset"), &m_pEmiter->m_nNumOfOneShootOffset, m_pEmiter->m_nNumOfOneShootOffset));
			m_pItemOmegaLX = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("omega x"), &m_pEmiter->m_fOmegaPitch_Local, m_pEmiter->m_fOmegaPitch_Local));
			m_pItemOmegaWY = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("omega y"), &m_pEmiter->m_fOmegaYaw_World, m_pEmiter->m_fOmegaYaw_World));
			m_pItemYawMin = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("yaw min"), &m_pEmiter->m_fMinYaw, m_pEmiter->m_fMinYaw));
			m_pItemYawMax = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("yaw max"), &m_pEmiter->m_fMaxYaw, m_pEmiter->m_fMaxYaw));			
			m_pItemPitchMin = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("pitch min"), &m_pEmiter->m_fMinPitch, m_pEmiter->m_fMinPitch));
			m_pItemPitchMax = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("pitch max"), &m_pEmiter->m_fMaxPitch, m_pEmiter->m_fMaxPitch));
			m_pItemGunsDir = (CXTPGI_RwV3d*)AddChildItem(new CXTPGI_RwV3d(_T("guns dir"), &m_pEmiter->m_vDir, m_pEmiter->m_vDir));
			m_pItemGunsSide = (CXTPGI_RwV3d*)AddChildItem(new CXTPGI_RwV3d(_T("guns side"), &m_pEmiter->m_vSide, m_pEmiter->m_vSide));
			m_pItemConeAngle = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("cone angle"), &m_pEmiter->m_fConAngle, m_pEmiter->m_fConAngle));

			static LPCTSTR	SZPGROUP[ePGroup_Num] = { "Point","Box","Cylinder","Sphere", };
			m_pItemPGroupType = (CXTPGI_Enum<ePGroup>*)AddChildItem(new CXTPGI_Enum<ePGroup>(_T("particle group"), ePGroup_Num, m_pEmiter->m_ePGroup, &m_pEmiter->m_ePGroup, SZPGROUP));

			switch( m_pEmiter->m_ePGroup ){
			case ePGroup_Point: break;
			case ePGroup_Box:
					m_pItemPGroupBox = (CXTPGI_PGBox*)AddChildItem(new CXTPGI_PGBox(&m_pEmiter->m_pgroupBox));
				break;
			case ePGroup_Cylinder:
					m_pItemPGroupCylinder = (CXTPGI_PGCylinder*)AddChildItem(new CXTPGI_PGCylinder(&m_pEmiter->m_pgroupCylinder));
				break;
			case ePGroup_Sphere:
					m_pItemPGroupSphere = (CXTPGI_PGSphere*)AddChildItem(new CXTPGI_PGSphere(&m_pEmiter->m_pgroupSphere));
				break;
			}

			//flags
			m_pItemBool_SHOOTRAND	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_SHOOTRAND	,_T("random shoot")));
			m_pItemBool_SHOOTCONE	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_SHOOTCONE	,_T("cone shoot")));
			m_pItemBool_YAWONEDIR	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_YAWONEDIR	,_T("y rot unidirectional")));
			m_pItemBool_YAWBIDIR	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_YAWBIDIR		,_T("y rot bidirectional")));
			m_pItemBool_PITCHONEDIR	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_PITCHONEDIR	,_T("x rot unidirectional")));
			m_pItemBool_PITCHBIDIR	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_PITCHBIDIR	,_T("x rot bidirectional")));
			m_pItemBool_NODEPND		= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_NODEPND		,_T("only move emiters pos")));
			m_pItemBool_SHOOTNOMOVE	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pEmiter->m_dwFlagOfEmiter, FLAG_EFFBASEPSYS_EMITER_SHOOTNOMOVE	,_T("stop particle")));
		};
};
//-----------------------------------------------------------------------------
// class CXTPGI_PSys::CXTPGI_PtcProp
//-----------------------------------------------------------------------------
class CXTPGI_PSys::CXTPGI_PtcProp : public CXTPPropertyGridItem
{
	AgcdEffParticleSystem::PARTICLEPROP*	m_pParticleProp;

	CXTPGI_Float*		m_pItemOmega;
	CXTPGI_Float*		m_pItemOmegaOffset;

	CXTPGI_UINT*		m_pItemOmegaLife;
	CXTPGI_UINT*		m_pItemOmegaLifeOffset;

	CXTPGI_CofEnvrn*	m_pItemCofEnvrn;
	CXTPGI_CofEnvrn*	m_pItemCofEnvrnOffset;
	//flags if any

public:
	CXTPGI_PtcProp(AgcdEffParticleSystem::PARTICLEPROP* pParticleProp)
		: CXTPPropertyGridItem(_T("particle prop"))
		, m_pParticleProp(pParticleProp)
	{
		ASSERT(m_pParticleProp);
	};
	virtual ~CXTPGI_PtcProp(){};

protected:
	virtual void OnAddChildItem()
	{
		ASSERT(m_pParticleProp);

		m_pItemOmega = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("omega"), &m_pParticleProp->m_fPAngularspeed, m_pParticleProp->m_fPAngularspeed));
		m_pItemOmegaOffset = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("omega offset"), &m_pParticleProp->m_fPAngularspeedOffset, m_pParticleProp->m_fPAngularspeedOffset));
		m_pItemOmegaLife = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("life"), &m_pParticleProp->m_dwParticleLife, m_pParticleProp->m_dwParticleLife));
		m_pItemOmegaLifeOffset = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("life offset"), &m_pParticleProp->m_dwParticleLifeOffset, m_pParticleProp->m_dwParticleLifeOffset));
		m_pItemCofEnvrn = (CXTPGI_CofEnvrn*)AddChildItem(new CXTPGI_CofEnvrn(_T("cof envrn"), &m_pParticleProp->m_stCofEnvrn));
		m_pItemCofEnvrnOffset = (CXTPGI_CofEnvrn*)AddChildItem(new CXTPGI_CofEnvrn(_T("cof envrn offset"), &m_pParticleProp->m_stCofEnvrnOffset));
		//flags
	};
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXTPGI_PSys::CXTPGI_PSys(CString strCaption, AgcdEffParticleSystem* pPSys)
	: CXTPGI_EffRenderBase(strCaption, pPSys)
	, m_pPSys(pPSys)
	, m_pItemCapacity(NULL)
	, m_pItemShootDelay(NULL)
	, m_pItemClumpName(NULL)
	, m_pItemEmiter(NULL)
	, m_pItemPtcProp(NULL)
{
	ASSERT(m_pPSys);
}
CXTPGI_PSys::~CXTPGI_PSys()
{
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CXTPGI_PSys::OnAddChildItem()
{
	CXTPGI_EffRenderBase::OnAddChildItem();

	ASSERT(m_pPSys);
	m_pItemCapacity = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("capacity"), m_pPSys->m_nCapacity, (LONG*)&m_pPSys->m_nCapacity));	
	m_pItemShootDelay = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("shoot delay"), &m_pPSys->m_dwShootDelay, m_pPSys->m_dwShootDelay));
	LPCTSTR	FILTER = _T("(*.rws)|*.rws|");
	m_pItemClumpName = (CXTPGI_FileName*)AddChildItem(new CXTPGI_FileName(_T("clump"), EFF2_FILE_NAME_MAX, m_pPSys->m_szClumpFName, m_pPSys->m_szClumpFName, AgcuEffPath::bGetPath_Clump(), FILTER ) );
	m_pItemEmiter = (CXTPGI_Emiter*)AddChildItem(new CXTPGI_Emiter(&m_pPSys->m_stEmiter));
	m_pItemPtcProp = (CXTPGI_PtcProp*)AddChildItem(new CXTPGI_PtcProp(&m_pPSys->m_stParticleProp));

	//flags
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_RANDCOLR			,_T("RANDCOLR"			)));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_INTERPOLATION		,_T("INTERPOLATION"		)));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_CHILDDEPENDANCY		,_T("CHILDDEPENDANCY"	)));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_PARTICLEBILLBOARD	,_T("PARTICLEBILLBOARD"	)));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_PARTICLEBILLBOARDY	,_T("PARTICLEBILLBOARDY")));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_CIRCLEEMITER		,_T("CIRCLEEMITER"		)));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_FILLCIRCLE			,_T("FILLCIRCLE"		)));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSys->m_dwBitFlags, FLAG_EFFBASEPSYS_CAPACITYLIMIT		,_T("CAPACITYLIMIT"		)));
};
