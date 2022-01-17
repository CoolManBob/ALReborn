// AcuSinTbl.h
// -----------------------------------------------------------------------------
//                       _____ _       _______ _     _     _     
//     /\               / ____(_)     |__   __| |   | |   | |    
//    /  \    ___ _   _| (___  _ _ __    | |  | |__ | |   | |__  
//   / /\ \  / __| | | |\___ \| | '_ \   | |  | '_ \| |   | '_ \ 
//  / ____ \| (__| |_| |____) | | | | |  | |  | |_) | | _ | | | |
// /_/    \_\\___|\__,_|_____/|_|_| |_|  |_|  |_.__/|_|(_)|_| |_|
//                                                               
//                                                               
//
// sin table
//
// -----------------------------------------------------------------------------
// Originally created on 02/19/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_ACUSINTBL_20050219
#define _H_ACUSINTBL_20050219

#ifndef NSACUMATH_BEGIN
#define NSACUMATH_BEGIN	namespace NSAcuMath {
#define NSACUMATH_END	};
#define USING_ACUMATH	using namespace NSAcuMath;
#define NSACUMATH		NSAcuMath
#endif

#include "AcuMathFunc.h"

NSACUMATH_BEGIN
// -----------------------------------------------------------------------------
// GAME PROGRAMMING Gems2, 2.1 부동수수점 비법들, page 244
class AcuSinTbl
{
public:
	AcuSinTbl();
	
	// Interface methods
	static inline const float Sin(float fdeg) 
	{
		const	float FTOIBIAS = 12582912.f; //1.5 * 2^23
		const	float COEF = DEF_D2R(fdeg)*256.f/DEF_2PI + FTOIBIAS;
		FNI		tmp(COEF);

		return sin[ (tmp.i & 0xff) ];
	}

	static inline const float Cos(float fdeg) 
	{
		const	float FTOIBIAS = 12582912.f; //1.5 * 2^23
		const	float COEF = DEF_D2R(fdeg)*256.f/DEF_2PI + FTOIBIAS + 64.f;
		FNI		tmp(COEF);

		return *(sin + (tmp.i & 0xff) );
	}

private:
	// Data members
	enum { E_TABLESIZE = 256 };
	static const AcuSinTbl* SINGLETON;
	static float sin[E_TABLESIZE];
};

// * ex : AcuSinTbl
// 최대 오차 : (+,-)0.0123f
//int _tmain(int argc, _TCHAR* argv[])
//{
//	argc;
//	argv;
//
//	cout << "----------------------------- sin --------------------------" << endl;
//	float fs = -43380.f;
//	float fe = 43380.f;
//	float fstep = 0.1f;
//	float fsoffset = 0.f;
//	float fcoffset = 0.f;
//	for( float f=fs; f<fe; f+=fstep )
//	{
//		float soff = sinf( DEF_D2R(f) ) - AcuSinTbl::Sin(f);
//		float coff = cosf( DEF_D2R(f) ) - AcuSinTbl::Cos(f);
//		if( fsoffset < fabsf(soff) )
//			fsoffset = fabsf(soff);
//		if( fcoffset < fabsf(coff) )
//			fcoffset = fabsf(coff);
//		printf( "deg : %6d"", sinf-sin[]: %8.4f"", cosf-cos[]: %8.4f\n", (int)f, soff, coff );
//	}
//	printf( "max sin-sin[] : %8.4f, max cos-cos[] : %8.4f\n", fsoffset, fcoffset );
//	cout << "----------------------------- sin --------------------------" << endl;
//	return 0;
//}

NSACUMATH_END
#endif // _H_ACUSINTBL_20050219
// -----------------------------------------------------------------------------
// AcuSinTbl.h - End of file
// -----------------------------------------------------------------------------

