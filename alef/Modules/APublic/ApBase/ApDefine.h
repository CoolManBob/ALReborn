#if !defined(__APDEFINE_H__)
#define __APDEFINE_H__

#if _MSC_VER >= 1400
#pragma warning (disable : 4996)
#endif

#include <winsock2.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#define	__ATTACH_LOGINSERVER__		//로그인 서버 사용여부이다.

//------------------------- type definition -------------------------
typedef signed char			INT8;
typedef short				INT16;
typedef	int					INT32;
typedef __int64				INT64;
typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef	unsigned int		UINT32;
typedef unsigned __int64	UINT64;
typedef	float				FLOAT;
typedef	char				CHAR;
typedef	void*				PVOID;
#define VOID				void		// void 는 typedef로 안돼나봐요;;

const INT32 INVALID_INDEX	= -1;

const int	OS_VERSION_WIN95_98_ME	= 0x01;
const int	OS_VERSION_WINNT_2000	= 0x02;
const int	ENCRYPT_STRING_SIZE		= 32;		//ID, Password를 인코딩하기위한 스트링의 사이즈.

#define		APDEFINE_MAX_OCCLUDER_BOX	8

// 패킷 우선순위 정의
enum PACKET_PRIORITY
{
	PACKET_PRIORITY_NONE	= -1,
	PACKET_PRIORITY_1,
	PACKET_PRIORITY_2,
	PACKET_PRIORITY_3,
	PACKET_PRIORITY_4,
	PACKET_PRIORITY_5,
	PACKET_PRIORITY_6,
	PACKET_PRIORITY_MAX
};

enum AuUnionType
{
	AUUNION_TYPE_NONE = 0,
	AUUNION_TYPE_RED_HEART,		//휴먼 
	AUUNION_TYPE_BLUE_BLOOD		//오크
};

enum AuRaceType
{
	AURACE_TYPE_NONE		= 0,
	AURACE_TYPE_HUMAN,			//휴먼 
	AURACE_TYPE_ORC,			//오크
	AURACE_TYPE_MOONELF,		//문엘프
	AURACE_TYPE_DRAGONSCION,	//드래곤시온
	AURACE_TYPE_MAX
};

enum AcCharGenderType
{
	AUCHARGENDER_TYPE_NONE		= 0,
	AUCHARGENDER_TYPE_MALE,
	AUCHARGENDER_TYPE_FEMALE,
	AUCHARGENDER_TYPE_MAX
};

enum AuCharClassType
{
	AUCHARCLASS_TYPE_NONE		= 0,
	AUCHARCLASS_TYPE_KNIGHT,
	AUCHARCLASS_TYPE_RANGER,
	AUCHARCLASS_TYPE_SCION,
	AUCHARCLASS_TYPE_MAGE,
	AUCHARCLASS_TYPE_MAX
};

enum  AuBlockingType
{
	AUBLOCKING_TYPE_NONE = 0,
	AUBLOCKING_TYPE_BOX,
	AUBLOCKING_TYPE_SPHERE,
	AUBLOCKING_TYPE_CYLINDER,
};

#define AUPOS_DISTANCE(pos1,pos2)		(sqrt(((pos1).x - (pos2).x) * ((pos1).x - (pos2).x) + ((pos1).y - (pos2).y) * ((pos1).y - (pos2).y) + ((pos1).z - (pos2).z) * ((pos1).z - (pos2).z)))
#define AUPOS_DISTANCE_XY(pos1,pos2)	(sqrt(((pos1).x - (pos2).x) * ((pos1).x - (pos2).x) + ((pos1).y - (pos2).y) * ((pos1).y - (pos2).y)))
#define AUPOS_DISTANCE_XZ(pos1,pos2)	(sqrt(((pos1).x - (pos2).x) * ((pos1).x - (pos2).x) + ((pos1).z - (pos2).z) * ((pos1).z - (pos2).z)))

struct AuPOS
{
	FLOAT	x;
	FLOAT	y;
	FLOAT	z;

	//AuPOS():x(0.0f),y(0.0f),z(0.0f){}
	// Union 사용을 위해 생성자가 있으면 에러남. by 마고자

	AuPOS operator-( AuPOS point )
	{
		AuPOS posReturn;
		posReturn.x = this->x - point.x;
		posReturn.y = this->y - point.y;
		posReturn.z = this->z - point.z;
		return posReturn;
	}

	AuPOS operator+( AuPOS point )
	{
		AuPOS posReturn;
		posReturn.x = this->x + point.x;
		posReturn.y = this->y + point.y;
		posReturn.z = this->z + point.z;
		return posReturn;
	}

	AuPOS operator*( FLOAT fValue )
	{
		AuPOS posReturn;
		posReturn.x = this->x * fValue;
		posReturn.y = this->y * fValue;
		posReturn.z = this->z * fValue;
		return posReturn;
	}

	FLOAT LengthXZ()
	{
		return sqrt( x * x + z * z );
	}

	FLOAT Length()
	{
		return sqrt( x * x + z * z + y * y );
	}

	void Normalize()
	{
		FLOAT fLen = Length();
		x /= fLen;
		y /= fLen;
		z /= fLen;
	}

	bool operator==( AuPOS point )
	{
		return point.x == x && point.z == z ? true : false;
	}

	BOOL	CollCylinder( FLOAT fWidth , FLOAT fWidthLine , AuPOS * pStart , AuPOS * pVelocity , AuPOS * pValid )
	{
		AuPOS vJoint;

		// 진행방향에 있는지 점검.
		{
			AuPOS	vNewDir;
			vNewDir = *this - *pStart;
			float  fDotProduct = ( vNewDir.x * pVelocity->x + vNewDir.z * pVelocity->z );
			if( fDotProduct <= 0 )		return FALSE;
		}

		float ftime;
		float ftime2;
		{
			AuPOS vVectorA;
			AuPOS vVectorB;

			vVectorA = *pStart - *this;
			vVectorB = *pVelocity;
			float fdistance = fWidth + fWidthLine;

			float fA2 = ( vVectorA.x * vVectorA.x + vVectorA.z * vVectorA.z );
			float fB2 = ( vVectorB.x * vVectorB.x + vVectorB.z * vVectorB.z );
			float fAB = ( vVectorA.x * vVectorB.x + vVectorA.z * vVectorB.z );

			float	fValueInTheSqrt;
			fValueInTheSqrt =	fAB * fAB - fB2 *( fA2 - fdistance * fdistance );

			if( fValueInTheSqrt < 0 ) return FALSE;

			ftime = ( - fAB - sqrt( fValueInTheSqrt ) ) / fB2;
			ftime2 = ( - fAB + sqrt( fValueInTheSqrt ) ) / fB2;
			// if( ftime < 0 ) continue;
			if( ftime > 1 ) return FALSE;
		}

		//matTmp.transform( &vLocal , &*this );

		if( ftime < 0 )
		{
			// 이미 블럭되어있는 상태다.
			// 윈위치로 놓음.
			*pValid = *pStart;
			return TRUE;
		}

		vJoint.x = pStart->x + ftime * pVelocity->x;
		vJoint.z = pStart->z + ftime * pVelocity->z;

		float fDistanceFromStart;
		fDistanceFromStart = AUPOS_DISTANCE_XZ( *pStart , vJoint );

		if( fDistanceFromStart < pVelocity->LengthXZ() )
		{
			*pValid = vJoint;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};
typedef AuPOS	AuV3D;

struct	AuLine
{
	AuPOS	start	;
	AuPOS	end		;

	AuPOS	GetNormal( bool bInv = false )
	{
		// XZ 평면에서의 노멀값
		// 진행방향의 왼쪽을 사용함.

		AuPOS vNormal;
		
		if( bInv )
			vNormal = end - start;
		else
			vNormal = start - end;

		vNormal.y = 0.0f;
		vNormal.Normalize();

		// 로테이트
		float fCos = 0;//cos( 90.0f );
		float fSin = 1;//sin( 90.0f );

		AuPOS ptTmp;

		#define ROTATE( pt ) {	ptTmp.x = fCos * ( pt ).x - fSin * ( pt ).z; \
								ptTmp.z = fSin * ( pt ).x + fCos * ( pt ).z; \
								pt = ptTmp; }

		ROTATE( vNormal )

		return vNormal;
	}

	BOOL	CollLine(  FLOAT fRadius , AuPOS * pStart , AuPOS * pVelocity , AuPOS * pValid )
	{
		BOOL bCollision = FALSE;

		float fMaxDistance = pVelocity->LengthXZ();
		float fNearestDistance = fMaxDistance;

		//for( int i = 0 ; i < 4 ; i ++ )
		{
			AuPOS	vNormal = this->GetNormal();
			float fDotN1V = ( vNormal.x * pVelocity->x + vNormal.z * pVelocity->z );

			AuPOS * pLineStart	= &start;
			AuPOS * pLineEnd	= &end	;

			/**
				2010. 04. 06. 역방향 충돌검사는 하지 않도록 수정합니다.
				그래픽팀에서 역방향 충돌검사를 하지 않는 것으로 알고 작업을 하셔서..
				역방향 충돌검사를 할 경우 갇혀버리는 곳이 종종 있습니다. By Somedail
			*/
			//if( fDotN1V > 0 )
			//{
			//	// 같은 방향인 경우도 충돌 체크한다.
			//	pLineStart	= &end	;
			//	pLineEnd	= &start;

			//	vNormal = this->GetNormal( true );// 역방향 노멀.
			//	fDotN1V = ( vNormal.x * pVelocity->x + vNormal.z * pVelocity->z );
			//}

			if( fDotN1V <= 0 )
			{
				// 0 이면 평행함.
				float fD = - ( vNormal.x * pLineStart->x + vNormal.z * pLineStart->z ) - fRadius;
				float t;

				t = - ( vNormal.x * pStart->x + vNormal.z * pStart->z + fD * 1 ) / fDotN1V ;

				if( t <= 0 )
				{
					AuPOS ptQ;
					ptQ.x = pStart->x;// + pVelocity->x * t;
					ptQ.z = pStart->z;// + pVelocity->y * t;
					ptQ.y = 0.0f;

					AuPOS vP0 , vV0;

					vP0 = *pLineStart;
					vV0 = *pLineEnd - *pLineStart; 

					AuPOS	vPQ;
					vPQ = ptQ - vP0;

					// 이 라인과의 거리 가 fRadius 보다 작으면..
					FLOAT fDistance = 
						sqrt( 
						( vPQ.x * vPQ.x + vPQ.z * vPQ.z ) -
						( vPQ.x * vV0.x + vPQ.z * vV0.z ) *
						( vPQ.x * vV0.x + vPQ.z * vV0.z ) /
						( vV0.x * vV0.x + vV0.z * vV0.z )
						);

					// 마고자
					// 플로트 오차때문에 10%의 보정값을 추가해서 계산함.
					fRadius *= 1.1f;

					if( fDistance <= fRadius )
					{
						// 내부 범위인지 확인
						AuPOS vA , vB;
						vA = *pLineEnd - *pLineStart;
						vB = *pStart - *pLineStart;

						FLOAT fDotA = ( vA.x * vB.x + vA.z * vB.z );

						vA = *pLineStart - *pLineEnd;
						vB = *pStart - *pLineEnd;

						FLOAT fDotB = ( vA.x * vB.x + vA.z * vB.z );

						if( fDotA >= 0 && fDotB >= 0 )
						{
							/*
								// 작업용 쏘스
								AuPOS	vLine1 = *pStart - start;
								AuPOS	vLine2 = *pStart - end	;

								AuPOS	vDir;
								vDir.x = vLine1.x + vLine2.x;
								vDir.y = vLine1.y + vLine2.y;

								float	fDot;
								fDot = vNormal.x * vDir.x + vNormal.y * vDir.y;
								if( fDot < 0 )
								{
									//개무시.
									bCollision = FALSE;
									return FALSE;
								}
								else
								{
									// 콜리젼..
									bCollision = TRUE;
									fNearestDistance = 0.0f;
									*pValid = *pStart;
									return TRUE;
								}
							*/

							AuPOS	vLine1 = *pStart - *pLineStart	;
							AuPOS	vLine2 = *pStart - *pLineEnd	;

							AuPOS	vDir;
							vDir.x = vLine1.x + vLine2.x;
							vDir.z = vLine1.z + vLine2.z;

							FLOAT	fDotDir = vNormal.x * vDir.x + vNormal.z * vDir.z;

							if( fDotDir < 0 )
							{
								//개무시.
								bCollision = FALSE;
								return FALSE;
							}
							else
							{
								// 콜리젼..
								bCollision = TRUE;
								fNearestDistance = 0.0f;
								*pValid = *pStart;
								return TRUE;
							}
						}
					}
				}

				if( 0 < t && t <= 1 )
				{
					AuPOS ptTmp;
					ptTmp.x = pStart->x + pVelocity->x * t;
					ptTmp.z = pStart->z + pVelocity->z * t;
					ptTmp.y = 0.0f;

					float fDistance = AUPOS_DISTANCE_XZ( *pStart , ptTmp );

					if( fDistance < fNearestDistance )
					{
						// 내부 범위인지 확인
						AuPOS vA , vB;
						vA = *pLineEnd - *pLineStart;
						vB = ptTmp - *pLineStart;

						//vA.Normalize();
						//vB.Normalize();

						FLOAT fDotA = ( vA.x * vB.x + vA.z * vB.z );

						vA = *pLineStart - *pLineEnd;
						vB = ptTmp - *pLineEnd;

						//vA.Normalize();
						//vB.Normalize();

						FLOAT fDotB = ( vA.x * vB.x + vA.z * vB.z );

						if( fDotA >= 0 && fDotB >= 0 )
						{
							// 콜리젼..
							bCollision = TRUE;
							fNearestDistance = fDistance;
							*pValid = ptTmp;
						}
					}
				}
			}
		}
		return bCollision;
	}

	BOOL CollCylinder( FLOAT fRadius , AuPOS * pStart , AuPOS * pValid )
	{
		BOOL bCollision = FALSE;
		AuPOS vVelocity2;
		vVelocity2 = *pValid - *pStart;
		bCollision |= start.CollCylinder( fRadius , 0.0f , pStart , &vVelocity2 , pValid );
		vVelocity2 = *pValid - *pStart;
		bCollision |= end.CollCylinder( fRadius , 0.0f , pStart , &vVelocity2 , pValid );

		return bCollision;
	}
};

struct AuPOSBaseMeter
{
	INT16	x;
	INT16	y;
	INT16	z;
};

struct AuBOX
{
	AuPOS	inf;
	AuPOS	sup;
};

struct AuSPHERE
{
	AuPOS	center;
	FLOAT	radius;
};

struct	AuCYLINDER
{
	AuPOS	center;
	FLOAT	radius;
	FLOAT	height;
};

struct AuMATRIX
{
    AuPOS	right;
    UINT32	flags;
    AuPOS	up;
    UINT32	pad1;
    AuPOS	at;
    UINT32	pad2;
    AuPOS	pos;
    UINT32	pad3;
};

struct AuBLOCKING
{
	AuBlockingType	type;
	union
	{
		AuBOX		box;
		AuSPHERE	sphere;
		AuCYLINDER	cylinder;
	}data;
};

UINT32	MakeRGBToUINT32( FLOAT r , FLOAT g , FLOAT b , FLOAT a = 0.0f);
bool	ReadUINT32ToRGB( UINT32 uRGB ,FLOAT *r , FLOAT *g , FLOAT *b , FLOAT *a = NULL);

struct AuCharacterLightInfo
{
	enum	TYPE
	{
		NO_VARIATION,
		AMBIENT_PLUS,
		AMBIENT_MINUS,
		DIRECTIONA_VARIATION,
	};

	TYPE		eType;
	FLOAT		r1;
	FLOAT		g1;
	FLOAT		b1;
	FLOAT		r2;
	FLOAT		g2;
	FLOAT		b2;
	UINT32		uChangeGap;

	TYPE	GetType()	{	return eType;	}

	void	Clear()
	{
		eType	= NO_VARIATION;
	}

	void	MakeString( char* pStr )
	{
		if( GetType() )
		{
			UINT32 uRGB1 = MakeRGBToUINT32( r1 , g1 , b1 );
			UINT32 uRGB2 = MakeRGBToUINT32( r2 , g2 , b2 );
			sprintf( pStr , "%d:%x:%x:%d", (INT32)eType, uRGB1, uRGB2, uChangeGap );
		}
		else
			sprintf( pStr , "%d" , ( INT32 ) eType );
	}

	void	ReadString( const char * pStr )
	{
		UINT32	uRGB1 , uRGB2;
		INT32	uType ;

		sscanf( pStr , "%d:%x:%x:%d" , &uType , &uRGB1 , &uRGB2 , &uChangeGap );
		eType = (TYPE) uType;

		ReadUINT32ToRGB( uRGB1 , &r1 , &g1 , &b1 );
		ReadUINT32ToRGB( uRGB2 , &r2 , &g2 , &b2 );
	}

	AuCharacterLightInfo & operator= ( const AuCharacterLightInfo & param )
	{
		eType		= param.eType;
		r1			= param.r1;	
		g1			= param.g1;	
		b1			= param.b1;	
		r2			= param.r2;	
		g2			= param.g2;	
		b2			= param.b2;	
		uChangeGap	= param.uChangeGap;	
		return *this;
	}
};

#define MAX(a,b)						((a) > (b) ? (a) : (b))
#define MIN(a,b)						((a) < (b) ? (a) : (b))
#define CLAMP( min, p, max )			(p) = MAX( (min), ( MIN( (p), (max) ) ) )

#define MIN3(a,b,c)						(MIN(MIN(a,b),c))
#define MAX3(a,b,c)						(MAX(MAX(a,b),c))
#define SWAP(a,b)						{(a)^=(b)^=(a)^=(b);}

#define AUPOS_EQUAL(pos1,pos2)			((pos1).x == (pos2).x && (pos1).y == (pos2).y && (pos1).z == (pos2).z)

#define	AUTEST_POS_IN_SPHERE(pos,sphere)		(AUPOS_DISTANCE((pos),(sphere).center) <= (sphere).radius)
#define AUTEST_POS_IN_CYLINDER(pos,cylinder)	((pos).y >= (cylinder).center.y && (pos).y <= (cylinder).center.y + (cylinder).height && AUPOS_DISTANCE_XY(pos,(cylinder).center) <= (cylinder).radius)
#define AUTEST_POS_IN_BOX(pos,box)				((pos).x >= (box).inf.x && (pos).y >= (box).inf.y && (pos).z >= (box).inf.z && (pos).x <= (box).sup.x && (pos).y <= (box).sup.y && (pos).z <= (box).sup.z)

#define AUMATRIX_EQUAL_POS(mat1,mat2)		AUPOS_EQUAL((mat1).pos,(mat2).pos)
#define AUMATRIX_EQUAL_RIGHT(mat1,mat2)		AUPOS_EQUAL((mat1).right,(mat2).right)
#define AUMATRIX_EQUAL_UP(mat1,mat2)		AUPOS_EQUAL((mat1).up,(mat2).up)
#define AUMATRIX_EQUAL_AT(mat1,mat2)		AUPOS_EQUAL((mat1).at,(mat2).at)

#define AUMATRIX_EQUAL(mat1,mat2)			(AUMATRIX_EQUAL_POS(mat1,mat2) && AUMATRIX_EQUAL_RIGHT(mat1,mat2) && AUMATRIX_EQUAL_UP(mat1,mat2) && AUMATRIX_EQUAL_AT(mat1,mat2))

#define AU_DEG2RAD							0.0174532925199f
#define AU_RAD2DEG							57.2957795132f

struct AuAREA
{
	AuPOS	posLeftTop;			// left-top position
	AuPOS	posRightBottom;		// right-bottom position
};

struct stDIRT
{
	INT16	nTotalPoint;
	INT16	nDuration;
	INT16	nIntensity;
	INT16	nRange;
	INT16	nTarget;
};

// packet type definition
///////////////////////////////////////////////////////////////////////////////
// 공통 모듈
enum ePacketType
{
	ALEF_SYSTEM_PACKET_TYPE							= 0x00,

	AGPMCONFIG_PACKET_TYPE							= 0x01,
	AGPMCHARACTER_PACKET_TYPE						= 0x02,
	AGPMITEM_PACKET_TYPE							= 0x04,
	AGPMPARTYITEM_PACKET_TYPE						= 0x29,
	AGPMSKILL_PACKET_TYPE							= 0x05,
	AGPMSHRINE_PACKET_TYPE							= 0x06,
	AGPMPRIVATETRADE_PACKET_TYPE					= 0x07,					// PrivateTrade Packet Type
	AGPMPARTY_PACKET_TYPE							= 0x08,					// party module packet type
	AGPMRECRUIT_PACKET_TYPE							= 0x0A,					// Recruit packet type
	AGPMAUCTION_PACKET_TYPE							= 0x0B,					// Auction module packet type
	AGPMSYSTEMINFO_PACKET_TYPE						= 0x0C,					// 시스템 정보를 Client로.....
	AGPMLOGIN_PACKET_TYPE							= 0x0D,					// Login정보 주고받기용
	AGPMOBJECT_PACKET_TYPE							= 0x0E,					// object client & server packet type
	AGPMCHATTING_PACKET_TYPE						= 0x0F,					// chatting packet type
	AGPMAI_PACKET_TYPE								= 0x10,					// ai information packet
	AGPMTIMER_PACKET_TYPE							= 0x11,					// Timer packet type
	AGPMADMIN_PACKET_TYPE							= 0x12,					// Admin Packet Type
	AGPMQUEST_PACKET_TYPE							= 0x13,					// Quest Packet Type
	AGPMUISTATUS_PACKET_TYPE						= 0x14,					// UI Status Module Packet Type
	AGPMGUILD_PACKET_TYPE							= 0x15,					// Guild Packet Type
	AGPMITEMCONVERT_PACKET_TYPE						= 0x16,					// Item Convert Module Packet Type
	AGPMPRODUCT_PACKET_TYPE							= 0x17,					// Product Module Packet Type
	AGPMWORLD_PACKET_TYPE							= 0x18,					// World Module Packet Type
	AGPMCASPER_PACKET_TYPE							= 0x19,					// Casper Module Packet Type
	AGPMREFINERY_PACKET_TYPE						= 0x4B,					// Refinery Module Packet Type
	AGPMPVP_PACKET_TYPE								= 0x4A,					// PvP Module Packet Type
	AGPMAREACHATTING_PACKET_TYPE					= 0x4C,
	AGPMBILLINFO_PACKET_TYPE						= 0x4E,					// billing information module
	AGPMRIDE_PACKET_TYPE							= 0x4D,					// Ride Module Packet Type
	AGPMSUMMONS_PACKET_TYPE							= 0x4F,					// Summons Module Packet Type
	AGPMREMISSION_PACKET_TYPE						= 0x2A,
	AGPMBUDDY_PACKET_TYPE							= 0x2B,					// Buddy Module Packet Type
	AGPMCHANNEL_PACKET_TYPE							= 0x2C,					// Channel Module Packet Type
	AGPMMAILBOX_PACKET_TYPE							= 0x2D,					// Mail Box Module Packet Type
	AGPMCASHMALL_PACKET_TYPE						= 0x2E,
	AGPMRETURNTOLOGIN_PACKET_TYPE					= 0x2F,
	AGPMWANTEDCRIMINAL_PACKET_TYPE					= 0x46,
	AGPMNATUREEFFECT_PACKET_TYPE					= 0x47,					// Nature Effect 뿌리기~
	AGPMSTARTUPENCRYPTION_PACKET_TYPE				= 0x48,
	AGPMSIEGEWAR_PACKET_TYPE						= 0x49,					// 공성 모듈
	AGPMSEARCH_PACKET_TYPE							= 0x52,					// 검색 패킷
	AGPMTAX_PACKET_TYPE								= 0x53,
	AGPMGUILDWAREHOUSE_PACKET_TYPE					= 0x54,
	AGPMARCHLORD_PACKET_TYPE						= 0x55,					// Archlord System Packet
	AGPMGAMBLE_PACKET_TYPE							= 0x56,					// gamble

	AGPMOPTIMIZEDCHARMOVE_PACKET_TYPE				= 0x40,					// AgpmOptimizedPacket Module Character Move Packet Type
	AGPMOPTIMIZEDCHARACTION_PACKET_TYPE				= 0x41,					// AgpmOptimizedPacket Module Character Action Packet Type
	AGPMOPTIMIZEDVIEW_PACKET_TYPE					= 0x42,					// AgpmOptimizedPacket Module Character, Item View Packet Type

	AGPMEVENT_NATURE_PACKET_TYPE					= 0x30,					// event module은 0x30부터 시작
	AGPMEVENT_TELEPORT_PACKET_TYPE					= 0x31,
	AGPMEVENT_NPCTRADE_PACKET_TYPE					= 0x32,					// NPC Trade Event
	AGPMEVENT_ITEMREPAIR_PACKET_TYPE				= 0x34,					// Item repair Event
	AGPMEVENT_MASTERY_SPECIALIZE_PACKET_TYPE		= 0x35,					// Mastery specialize event
	AGPMEVENT_BANK_PACKET_TYPE						= 0x36,					// bank event
	AGPMEVENT_NPCDIALOG_PACKET_TYPE					= 0x37,					// NPC Dialog Event
	AGPMEVENT_ITEMCONVERT_PACKET_TYPE				= 0x38,					// item convert event
	AGPMEVENT_GUILD_PACKET_TYPE						= 0x39,					// Guild Event
	AGPMEVENT_PRODUCT_PACKET_TYPE					= 0x3A,					// Product Event
	AGPMEVENT_SKILLMASTER_PACKET_TYPE				= 0x3B,					// Skill Master Event
	AGPMEVENT_MANAGER								= 0x3C,
	AGPMEVENT_QUEST_PACKET_TYPE						= 0x3E,					// Quest Event
	AGPMEVENT_REFINERY_PACKET_TYPE					= 0x3F,					// Refinery Event
	AGPMEVENT_CHARCUSTOMIZE_PACKET_TYPE				= 0x43,					// character customize
	AGPMEVENT_SIEGEWAR_NPC_PACKET_TYPE				= 0x50,					// siegewar npc event module
	AGPMEVENT_GACHA_PACKET_TYPE						= 0x57,					// Gacha Event

	AGPMSYSTEMMESSAGE_PACKET_TYPE					= 0x44,					// System Message, 20050906, kelovon
	AGPMSCRPIT_PACKET_TYPE							= 0x45,					// Script command
	AGPM_BATTLEGROUND_PACKET_TYPE					= 0x58,					// Gacha Event
	AGPM_EPICZONE_PACKET_TYPE						= 0x59,					// EpicZone Event
	AGPM_TITLE_PACKET_TYPE							= 0x60,					// Title Event

	// 클라이언트 모듈
	AGCMCHAR_PACKET_TYPE							= 0x03,
	AGCMCONNECTMANAGER_PACKET_TYPE					= 0x1B,
	AGCMPARTY_PACKET_TYPE							= 0x09,

	// 서버 모듈
	AGSMCHARMANAGER_PACKET_TYPE						= 0x03,
	AGSMPARTY_PACKET_TYPE							= 0x09,
	AGSMGLOBALCHATTING_PACKET_TYPE					= 0x51,

	// 서버간 통신
	AGSMSERVER_PACKET_TYPE							= 0x1A,
	AGSMCHARACTER_PACKET_TYPE						= 0x1B,					// 서버간 통신은 0x1A부터...
	AGSMACCOUNT_PACKET_TYPE							= 0x1C,					// account manager가 사용
	AGSMRECRUIT_PACKET_TYPE							= 0x1D,					// Recruit서버와 게임서버간 통신.
	AGSMAUCTION_PACKET_TYPE							= 0x1E,					// Auction서버와 게임서버간 통신.
	AGSMITEM_PACKET_TYPE							= 0x1F,
	AGSMSKILL_PACKET_TYPE							= 0x20,
	AGSMDEATH_PACKET_TYPE							= 0x21,
	AGSMLOGIN_PACKET_TYPE							= 0x22,
	AGSMZONING_PACKET_TYPE							= 0x23,
	AGSMADMIN_PACKET_TYPE							= 0x24,
	AGSMRELAY_PACKET_TYPE							= 0x25,
	AGSMITEMLOG_PACKET_TYPE							= 0x26,
	AGSMLOG_PACKET_TYPE								= 0x27,
	AGSMGUILD_PACKET_TYPE							= 0x28,
};

// const variable definition
///////////////////////////////////////////////////////////////////////////////
const int AP_INVALID_AID								= 0;
const int AP_INVALID_CID								= 0;
const int AP_INVALID_IID								= 0;
const int AP_INVALID_PARTYID							= 0;
const int AP_INVALID_SERVERID							= 0;
const int AP_INVALID_SKILLID							= 0;
const int AP_INVALID_SHRINEID							= 0;


// AuBplusTree
///////////////////////////////////////////////////////////////////////////////
const int AUBTREE_FLAG_KEY_NUMBER						= 0;
const int AUBTREE_FLAG_KEY_STRING						= 1;

const int AUBTREE_MAX_KEY_STRING						= 40;

// LOD (Bob님 작업, 122902)
///////////////////////////////////////////////////////////////////////////////
const int AGPDLOD_MAX_NUM								= 5;

// TEMPORARY TEMPLATE
///////////////////////////////////////////////////////////////////////////////
const int AGPD_TEMPORARY_TEMPLATE_INDEX					= 44444;

//Login
const int AGPMLOGIN_MAX_ACCOUNTID						= 32;
const int AGPMLOGIN_MAX_PASSWORD						= 16;

// AgpmCharacter ( public Character module )
///////////////////////////////////////////////////////////////////////////////
const int AGPACHARACTER_MAX_CHARACTER_TITLE				= 32;
const int AGPACHARACTER_MAX_CHARACTER_SKILLINIT			= 32;
const int AGPACHARACTER_MAX_CHARACTER_NICKNAME			= 16;
const int AGPACHARACTER_INVENTORY_NUM					= 3;		//인벤토리의 갯수 
const int AGPACHARACTER_INVENTORY_ROW1					= 10;		//인벤토리의 가로 칸수 
const int AGPACHARACTER_INVENTORY_ROW2					= 7;		//인벤토리의 세로 칸수 
const int AGPACHARACTER_CHARACTER_DATA_COUNT			= 1000;		//최대 Character수 
const int AGPACHARACTER_BTREE_ORDER						= 20;		//B Tree ORDER
const int AGPACHARACTER_MAX_ID_STRING					= 48;		//게임 아이디의 최대 길이
const int AGPACHARACTER_MAX_PW_STRING					= 32;		//게임 패스워드의 최대길이.
const int AGPACHARACTERT_MAX_TEMPLATE_NAME				= 32;		//CharacterTemplate의 이름 
const int AGPACHARACTERT_TEMPLATE_DATA_COUNT			= 100;		//메모리에 저장할 템플릿의 갯수 
const int AGPACHARACTERT_BTREE_ORDER					= 20;		//B Tree ORDER

const int AGPMCHARACTER_MAX_EQUIP_ITEMLIST				= 10;
const int AGPMCHARACTER_MAX_DROP_ITEMLIST				= 10;

enum eAGPMCHARACTER_TEMPLATE_RACE_TYPE		// character race type
{
	AGPMCHARACTER_TYPE_HUMAN	= 1,
	AGPMCHARACTER_TYPE_ORC		= 2,
	AGPMCHARACTER_TYPE_NPC		= 3,
	AGPMCHARACTER_TYPE_MONSTER	= 4
};

// AgpmSkill	( public Skill module )
///////////////////////////////////////////////////////////////////////////////
const int AGPMSKILL_MAX_SKILL_NAME						= 64;

const int AGPDSKILL_MAX_SKILLT_COMMENT					= 30;		//Template의 Comment 최대 길이 
const int AGPDSKILL_MAX_VISUAL_EFFECT_ID				= 10;		//Template에서 한 SKill이 가질 수 있는 Effect(Visual)의 최대수 
const int AGPASKILLT_TEMPLATE_DATA_COUNT				= 100;		//Skill Template의 수 
const int AGPASKILLT_BTREE_ORDER						= 10;		//B TREE ORDER
const int AGPMSKILL_MAX_BUFF							= 10;

enum eAGPMSKILL_BUFF_TYPE	// buff type
{
	AGPMSKILL_BUFF_JOIN_PARTY		= 1,
	AGPMSKILL_BUFF_PARTY_AC_UP		= 2,
	AGPMSKILL_BUFF_PARTY_DMB_UP		= 3
};


// AgpmItem (public item module)
///////////////////////////////////////////////////////////////////////////////
const __int64 AGPMITEM_MAX_MONEY						= 999999999999; //9999억 9999만 9999
const int	AGPM_TRADE_MAX_MONEY						= 999999999;	//9억 9999만 9999

// AgpmParty (public party module)
///////////////////////////////////////////////////////////////////////////////
const int AGPMPARTY_MAX_PARTY_NAME						= 40;
const int AGPMPARTY_MAX_PARTY_MEMBER					= 5;
const int AGPMPARTY_MAX_DATA_COUNT						= 1000;
const int AGPMPARTY_MAX_EFFECT							= 10;


// AgpmShrine (public shrine module)
///////////////////////////////////////////////////////////////////////////////
const int AGPMSHRINE_MAX_SHRINE_NAME					= 40;
const int AGPMSHRINE_MAX_GUARDIAN						= 3;
const int AGPMSHRINE_MAX_SHRINE_DATA_COUNT				= 40;
const int AGPMSHRINE_MAX_SHRINE_POSITION				= 4;

const int AGPMSHRINE_TYPE_ALWAYS						= 0x001;
const int AGPMSHRINE_TYPE_ACTIVE_BY_COND				= 0x002;
const int AGPMSHRINE_TYPE_RANDOM_POS					= 0x004;

const int AGPMSHRINE_TYPE_COND_NONE						= 0x010;
const int AGPMSHRINE_TYPE_KILL_GUARD					= 0x020;
const int AGPMSHRINE_TYPE_HAVE_RUNE						= 0x040;
const int AGPMSHRINE_TYPE_WITH_PARTY					= 0x080;

const int AGPMSHRINE_TYPE_ACTIVE						= 1;
const int AGPMSHRINE_TYPE_INACTIVE						= 2;
const int AGPMSHRINE_TYPE_VANISH						= 3;

// Range Checking Inline functions..
inline bool AuIsInBBox( AuBOX box , AuPOS pos )
{
	if( box.inf.x <= pos.x && box.sup.x > pos.x &&
		box.inf.y <= pos.y && box.sup.y > pos.y &&
		box.inf.z <= pos.z && box.sup.z > pos.z	)
		return true;
	else
		return false;
}

// Do not check the Y factor , Only determine whether position is in bbox x-y plane
inline bool AuIsInBBoxPlane( AuBOX box , AuPOS pos )
{
	if( box.inf.x <= pos.x && box.sup.x > pos.x &&
		//box.inf.y <= pos.y && box.sup.y > pos.y &&
		box.inf.z <= pos.z && box.sup.z > pos.z	)
		return true;
	else
		return false;
}

typedef BOOL ( * ProgressCallback )
    ( char * strMessage , int nCurrent , int nTarget , void *data );

// 마고자 (2004-03-23 오후 5:03:11) : 
// FLOAT -> INT 전환 퍼포먼스 개선..
	union INTORFLOAT	// FtoI 퍼포먼스 개선용 union
	{
		int      i;
		float    f;
		INTORFLOAT(): i( ( 23 + 127 ) << 23 ){}
	};

	// 데이타 처리용 임시 글로벌 변수..
	// MagDebug.cpp 에 정의되어있음.
	extern INTORFLOAT	g_unionBiasForFtoI;
	extern INTORFLOAT	g_unionTempForFtoI;

	// 처리용 Define 문..
	// IEEE 포인터 트릭 ( GPG2 참조 .. )
	// 을이용한 매크로 함수. 
	// IToF는 이방법을 이용하는것보다 그냥 캐스팅 하는게 빨라서 의미가 없으나.
	// 이름을 통일함..
	// 차후 추가할지도 모름
	#define MagFIsMinus( fValue )	( g_unionTempForFtoI.f = (fValue) , g_unionTempForFtoI.i < 0 )
	#define MagFToI( fValue )		( ( g_unionTempForFtoI.f = ( MagFIsMinus( fValue ) ? g_unionTempForFtoI.f * 0.5f : g_unionTempForFtoI.f ) + g_unionBiasForFtoI.f ) , ( g_unionTempForFtoI.i	-= g_unionBiasForFtoI.i	) < 0 ? g_unionTempForFtoI.i >> 1 : g_unionTempForFtoI.i )
	//#define MagIToF( nValue )		( ( g_unionTempForFtoI.i = nValue + g_unionBiasForFtoI.i ) , ( g_unionTempForFtoI.f	-= g_unionBiasForFtoI.f	) )
	#define MagIToF( nValue )		( ( FLOAT ) ( nValue ) )
	#define MagFABS( fValue )		( (fValue) > 0 ? (fValue) : -(fValue) )

// 풀패스에서 패스 제거..
inline const char * __RemovePath( const char * pPath )
{
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( pPath , drive, dir, fname, ext );
	return pPath + strlen( drive ) + strlen( dir );
}

enum eAccountLevel
{
	ACCOUNT_LEVEL_NONE = 0,
	ACCOUNT_LEVEL_CLOSE_BETA = 5,
	ACCOUNT_LEVEL_DEVELOPER = 70,
	ACCOUNT_LEVEL_ADMIN = 88,
};

// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////
// macro
// /////////////////////////////////////////////////////////////////////////////
// *macro constants
#define DEF_PI		3.141592653589793f
#define DEF_2PI		6.283185307179587f
#define DEF_DTOR	0.017453292519943f			// DEF_PI/180
#define DEF_RTOD	57.29577951308232f			// 180/DEF_PI
// *macro func
#define DEF_D2R(deg)	( (deg) * DEF_DTOR )	// degree -> radian
#define DEF_R2D(rad)	( (rad) * DEF_RTOD )	// radian -> degree

#define DEF_ALIGN32(a)	( ((a)+31) & (~31) )	// b = DEF_ALIGN32(a) -> (b >= a) AND ((b % 32) == 0)
#define DEF_ALIGN16(a)	( ((a)+15) & (~15) )	// b = DEF_ALIGN16(a) -> (b >= a) AND ((b % 16) == 0)
#define DEF_ALIGN8(a)	( ((a)+7 ) & (~7 ) )	// b = DEF_ALIGN8(a)  -> (b >= a) AND ((b % 8 ) == 0)
#define DEF_ALIGN4(a)	( ((a)+3 ) & (~3 ) )	// b = DEF_ALIGN4(a)  -> (b >= a) AND ((b % 4 ) == 0)
#define DEF_ALIGN2(a)	( ((a)+1 ) & (~1 ) )	// b = DEF_ALIGN2(a)  -> (b >= a) AND ((b % 2 ) == 0)

#define DEF_ISODD(a)	( (a) & 0x1 )			// if( DEF_XXX(a) == TRUE	) --> err
#define DEF_ISMINUS4(a)	( (a) & 0x80000000 )	// if( DEF_XXX(a) == FALSE	) --> err
#define DEF_ISMINUS2(a)	( (a) & 0x8000     )	// if( DEF_XXX(a) )			  --> only use in this form
#define DEF_ISMINUS1(a)	( (a) & 0x80       )

#define DEF_CLAMP(val, min, max)	( ((val)<(min)) ? ((val)=(min)) : ( ((val)>(max)) ? ((val)=(max)) : (val) ) )

#define DEF_FLAG_CHK(val, flag)		( (val) & (flag) )
#define DEF_FLAG_ON(val, flag)		( (val) |= (flag) )
#define DEF_FLAG_OFF(val, flag)		( (val) &= ~(flag) )

#define DEF_RGB32(r,g,b)			( (BYTE)(b) + ((BYTE)(g)<<8) + ((BYTE)(r)<<16) )
#define DEF_ARGB32(a,r,g,b)			( (BYTE)(b) + ((BYTE)(g)<<8) + ((BYTE)(r)<<16) + ((BYTE)(a)<<24) )

#define DEF_SET_ALPHA(color, alpha)	( ( (color) &= 0x00ffffff ) |= ( (BYTE)(alpha) << 24 ) )
#define DEF_SET_RED(color, red)		( ( (color) &= 0xff00ffff ) |= ( (BYTE)(red  ) << 16 ) )
#define DEF_SET_GREEN(color, green)	( ( (color) &= 0xffff00ff ) |= ( (BYTE)(green) << 8  ) )
#define DEF_SET_BLUE(color, blue)	( ( (color) &= 0xffffff00 ) |= ( (BYTE)(blue )       ) )

#define DEF_GET_ALPHA(color)		(UINT8)( ( (color) & 0xff000000 ) >> 24 )
#define DEF_GET_RED(color)			(UINT8)( ( (color) & 0x00ff0000 ) >> 16 )
#define DEF_GET_GREEN(color)		(UINT8)( ( (color) & 0x0000ff00 ) >> 8  )
#define DEF_GET_BLUE(color)			(UINT8)( ( (color) & 0x000000ff )		)

#define	DEF_SAFEDELETE(a)			if(a){ delete (a); (a)=NULL;}
#define DEF_SAFEDELETEARRAY(a)		if(a){ delete [] (a); (a)=NULL;}
#define DEF_SAFERELEASE(a)			if(a){ (a)->Release(); (a)=NULL;}
#define DEF_SAFEFCLOSE(fp)			if(fp){ fclose(fp); (fp)=NULL;}

#define Eff2Ut_ZEROBLOCK(t)			{ ZeroMemory( &t,sizeof(t) ); }

// key state
static inline BOOL KEYDOWN( DWORD vk_code )	
{ 
	return ( ( GetAsyncKeyState( vk_code ) & 0x8000 ) ? TRUE  : FALSE); 
}
static inline BOOL KEYUP( DWORD vk_code )	
{ 
	return ( ( GetAsyncKeyState( vk_code ) & 0x8000 ) ? FALSE : TRUE) ; 
}
static inline BOOL KEYDOWN_ONECE( DWORD vk_code, BOOL &bFlag)
{// bFlag는 전역변수이거나, static 변수.
	if(bFlag)
		if( KEYUP( vk_code ) )	
			return (bFlag = FALSE);
		else		
			return  FALSE;
	else	
		return (bFlag = KEYDOWN( vk_code ));
}
// * ex : KEYDOWN_ONECE
//int _tmain(int argc, _TCHAR* argv[])
//{
//	argc;
//	argv;
//	//1 을 누른 상태에서 ENTER
//	int tmp = 0;
//	cin >> tmp;
//	cout << "tmp : " << tmp << endl;
//	for( int i=0; i<100; ++i )
//	{
//		static BOOL b1 = FALSE;
//		if( KEYDOWN_ONECE('1', b1) )
//			cout << "KEYDOWN_ONECE('1', b1) == TRUE" << endl;
//	}
//
//	if( KEYDOWN('2') )
//		cout << "KEYDOWN('2') == TRUE" << endl;
//	if( KEYUP('2') )
//		cout << "KEYUP('2') == TRUE" << endl;
//		
//	return 0;
//}

// 160205(BOB & kday)
class AuAutoFile
{
	FILE* fp;
private:
	//Do not use!
	AuAutoFile(const AuAutoFile& cpy){cpy;};
	AuAutoFile& operator = (const AuAutoFile& cpy){cpy; return *this;}
public:
	AuAutoFile(LPCSTR fname, LPCSTR mode)
		: fp( fopen(fname, mode) ){}
	~AuAutoFile(){ DEF_SAFEFCLOSE(fp); }
	operator FILE* (){return fp;}
	FILE* operator ->() {return fp;}
};
// * ex : AuAutoFile
//int _tmain(int argc, _TCHAR* argv[])
//{
//	argc;
//	argv;
//
//	{
//		AuAutoFile	fp("test.txt", "wt");
//		if(fp)
//		{
//			int n=20;
//			fwrite(&n, sizeof(n), 1, fp);
//		}
//	}
//	
//	{
//		AuAutoFile	fp("test.txt", "rt");
//		if(fp)
//		{
//			int n=30;
//			fread(&n, sizeof(n), 1, fp);
//			cout << n << endl;
//
//		}
//	}
//	return 0;
//}

// 마고자 (2005-11-21 오전 11:07:08) : 
// 마고자 싱글톤 구현 추가..
// 컨스트럭터는 따로 정의해 주어야함.
// 다음과 같은 매크로로 예전 코드와 호완
// #define m_pcsAgcmMap (&AgcmMap::GetInstance())
#define	DECLARE_SINGLETON( classname )		\
public:										\
	static classname & GetInstance()		\
	{										\
		static classname _sStaticInstance;	\
		return _sStaticInstance;			\
	}										\
private:									\
	classname();							\
	classname( const classname &){}			\
public:

// 국가 설정값.
typedef enum
{
	AP_SERVICE_AREA_KOREA	= 0,
	AP_SERVICE_AREA_CHINA	= 1,
	AP_SERVICE_AREA_WESTERN	= 2,
	AP_SERVICE_AREA_JAPAN	= 3
} ApServiceArea;

// 각각 국가 플래그 사용의 경우

extern const ApServiceArea g_eServiceArea;
bool IsChina();
bool IsWestern();
bool IsKorea();
bool IsJapan();

#define GETSERVICEAREAFLAG( eArea ) ( 0x0001 << (eArea) )

extern INT32 g_lLocalServer;

int	stl_printf( std::string str , const char * pFormat , ... );

#endif // __APDEFINE_H__
