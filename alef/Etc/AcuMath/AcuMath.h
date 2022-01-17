#ifndef		_ACUMATH_H_
#define		_ACUMATH_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuMathD" )
#else
#pragma comment ( lib , "AcuMath" )
#endif
#endif

#include "ApBase.h"

//class	AcuMath
//{
//public:
//	AcuMath();
//	~AcuMath();
//
//	static	void	Init();
//	static	void	Release();
//
//	static	FLOAT	GetSinValClampTest(int	deg);
//	static	FLOAT	GetSinValFast(int	deg);
//
//	static	FLOAT	GetCosValClampTest(int	deg);
//	static	FLOAT	GetCosValFast(int	deg);
//public:
//
//	static	FLOAT		m_fSinTable[360];		// 0 - 360' 의 sin table을 미리 갖는다.
//	static	FLOAT		m_fCosTable[360];		// 0 - 360' 의 cos table을 미리 갖는다.
//};
#ifndef NSACUMATH_BEGIN
#define NSACUMATH_BEGIN	namespace NSAcuMath {
#define NSACUMATH_END	};
#define USING_ACUMATH	using namespace NSAcuMath;
#define NSACUMATH		NSAcuMath
#endif

NSACUMATH_BEGIN

class AgcuAccmOnePlusSin
{
public:
	float	theta0;	//[rad]
	float	coef;	//amplitude/(theta1-theta0 - (cosf(theta1)-cosf(theta0)))
	float	speed;	//[rad/sec]  (theta1-theta0)/duration	
	float	theta;	//[rad]  theta0 + speed * accumulated_time 

	float	duration;	//[sec]
	float	accumtick;	//[sec]
public:
	AgcuAccmOnePlusSin(float deg0, float deg1, float amplitude, float holdtime/*[sec]*/ );
	bool	bOnIdle(float* pval, float diftick/*[sec]*/);
};
// * ex : AgcuAccmOnePlusSin
//#include <windows.h>
//void showRet(int ret)
//{
//	cout << ret << " : ";
//
//	while(ret > 0)
//	{
//		cout << '*';
//		ret -= 1;
//	}
//
//	cout << endl;
//}
//int _tmain(int argc, _TCHAR* argv[])
//{
//	unsigned beforetick = GetTickCount();
//
//	AgcuAccmOnePlusSin	tst(90.f, 270.f, 100.f, 0.5f);
//	float	theval = 0.f;
//	bool	bend = false;
//	do{
//		unsigned curtick = GetTickCount();
//		float diftick_sec = (curtick - beforetick)*0.001f;
//		beforetick = curtick;
//		bend = tst.bOnIdle( &theval, diftick_sec );
//		int castval = static_cast<int>(theval);
//		showRet(castval);
//	}while(!bend);
//
//	return 0;
//}

NSACUMATH_END

#endif