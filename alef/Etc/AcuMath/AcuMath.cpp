#include "AcuMath.h"
//
//FLOAT		AcuMath::m_fSinTable[360];
//FLOAT		AcuMath::m_fCosTable[360];
//
//AcuMath::AcuMath()
//{
//
//}
//
//AcuMath::~AcuMath()
//{
//
//}
//
//void AcuMath::Init()
//{
//	double	rad;
//	for(int	i=0;i<360;++i)
//	{
//		rad = (double)i * AU_DEG2RAD;
//
//		m_fSinTable[i] = (float)sin(rad);
//		m_fCosTable[i] = (float)cos(rad);
//	}
//}
//
//void AcuMath::Release()
//{
//
//}
//
//FLOAT	AcuMath::GetSinValClampTest(int	nDeg)
//{
//	if( nDeg & 0x80000000 ){
//		while( nDeg & 0x80000000 )
//			nDeg += 360;
//	}else{				
//		while( nDeg > 359 )
//			nDeg -= 360;
//	}					
//
//	return	m_fSinTable[nDeg];
//}
//
//FLOAT	AcuMath::GetSinValFast(int	nDeg)
//{
//	return	m_fSinTable[nDeg];
//}
//
//FLOAT	AcuMath::GetCosValClampTest(int	nDeg)
//{
//	if( nDeg & 0x80000000 ){
//		while( nDeg & 0x80000000 )
//			nDeg += 360;
//	}else{				
//		while( nDeg > 359 )
//			nDeg -= 360;
//	}					
//
//	return	m_fCosTable[nDeg];
//}
//
//FLOAT	AcuMath::GetCosValFast(int	nDeg)
//{
//	return	m_fCosTable[nDeg];
//}
//

#include "AcuSinTbl.h"

NSACUMATH_BEGIN

AgcuAccmOnePlusSin::AgcuAccmOnePlusSin(float deg0, float deg1, float amplitude, float holdtime/*[sec]*/ )
	: theta0( deg0 )
	, duration(holdtime)
	, accumtick(0.f)
{
	speed = (deg1 - theta0)/(duration);
	coef = amplitude/( (deg1 - theta0) - (AcuSinTbl::Cos(deg1)-AcuSinTbl::Cos(theta0)) );
};

bool AgcuAccmOnePlusSin::bOnIdle(float* pval, float diftick/*[sec]*/)
{
	bool br = false;
	accumtick += diftick;
	if( accumtick >= duration )
	{
		accumtick = duration;
		br = true;				//end
	}
	float theta = theta0 + speed * accumtick;

	*pval = coef * ((theta-theta0) - (AcuSinTbl::Cos(theta)-AcuSinTbl::Cos(theta0)));

	return br;
};

NSACUMATH_END