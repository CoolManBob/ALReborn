#ifndef	_H_AGCDEFFPOSTFX_20050421
#define _H_AGCDEFFPOSTFX_20050421

#include "AgcuEff2ApMemoryLog.h"
#include "AgcdEffBase.h"

enum	eFXType
{
	e_fx_unknown		= -1,
	e_fx_ClampingCircle	= 0,
	e_fx_Wave			,
	e_fx_Shockwave		,
	e_fx_Darken			,
	e_fx_Brighten		,
	e_fx_Ripple			,
	e_fx_Twist			,

	e_fx_typenum		,
};

struct	stSharedParam
{
	//RwReal	radius;		//예외 : ripple	--> from animation
	RwV2d	center;		//예외 : ripple
	eFXType	type;
};

struct	stWaveParam
{
	RwReal	amplitude;
	RwReal	frequency;
	RwChar	tex[EFF2_FILE_NAME_MAX];
};

struct stShockwaveParam
{
	RwReal	bias;
	RwReal	width;
	RwChar	tex[EFF2_FILE_NAME_MAX];
};

struct stRippleParam
{
	RwReal	scale;
	RwReal	frequency;
};

class AgcdEffPostFX : public AgcdEffBase
{
	EFFMEMORYLOG_SMV;

	static LPCSTR	tech[e_fx_typenum];

public:
	AgcdEffPostFX();
	virtual	~AgcdEffPostFX();

	// set
	void	bSetSharedParam(const RwV2d& center, eFXType type);
	void	bSetWaveParam(RwReal amplitude, RwReal frequency, LPCSTR tex);
	void	bSetShockwaveParam(RwReal bias, RwReal width, LPCSTR tex);
	void	bSetRippleParam(RwReal scale, RwReal frequency);
	// get
	const stSharedParam&	bGetSharedParam(void)const		{	return m_sharedparam;				}
	const stWaveParam&		bGetWaveParam(void)const		{	return m_unionparam.wave;			}
	const stShockwaveParam&	bGetShockwaveParam(void)const	{	return m_unionparam.shockwave;		}
	const stRippleParam&	bGetRippleParam(void)const		{	return m_unionparam.ripple;			}
	LPCSTR					bGetTech(void)const				{	return tech[m_sharedparam.type];	}

	RwInt32	bToFile(FILE* fp);
	RwInt32 bFromFile(FILE* fp);
	
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

	//do not use!
	AgcdEffPostFX( const AgcdEffPostFX& cpy ) : AgcdEffBase(cpy) {cpy;}
	AgcdEffPostFX& operator = ( const AgcdEffPostFX& cpy ) { cpy; return *this; }
	
public:

	stSharedParam	m_sharedparam;
	union
	{
		stWaveParam			wave;
		stShockwaveParam	shockwave;
		stRippleParam		ripple;
	}m_unionparam;
};

#endif