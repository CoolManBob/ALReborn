#ifndef __AGPDPATHFIND__
#define __AGPDPATHFIND__

#define		AGPMPATHFIND_RADIUS			50	//길찾기 메모리블럭의 반지름.
#define		AGPMPATHFIND_TRY_COUNT		100 //길찾기를 시도할 횟수.
#define		AGPMPATHFIND_BLOCKSIZE		200.0f //아크로드에서 기본 단위는 미터는 2M를 나타나려면 200을 입력한다.

typedef enum eAgpdPathFindBlockStatus
{
	AGPD_PATHFIND_BLOCK = 0,
	AGPD_PATHFIND_OPEN,
	AGPD_PATHFIND_CLOSE,
	AGPD_PATHFIND_NOTUSED,
};

typedef enum dAgpdPathFindResult
{
	AGPD_PATHFIND_ERROR = -1,
	AGPD_PATHFIND_FAIL,
	AGPD_PATHFIND_SUCCEED,
};

class AgpdPathFindPoint
{
public:
	int			m_iX, m_iY;
	float		m_fX, m_fY;

	AgpdPathFindPoint()
	{
		Reset();
	}
	AgpdPathFindPoint( int iX, int iY )
	{
		m_iX = iX;
		m_iY = iY;
	}
	void AgpdPathFindPoint::Reset()
	{
		m_iX = -1;
		m_iY = -1;
	}
	bool AgpdPathFindPoint::operator==( const AgpdPathFindPoint &cTempPoint )
	{
		if( (m_iX == cTempPoint.m_iX) && (m_iY == cTempPoint.m_iY) )
		{
			return true;
		}

		return false;
	}
};

class AgpdPathFindBlock
{
public:
	unsigned int	m_iRevision;
	unsigned char	m_iStatus;

	AgpdPathFindPoint  m_cParent;

	float			m_fCurrentX;
	float			m_fCurrentY;

	int				m_iPathCost;

	AgpdPathFindBlock()
	{
		m_iStatus = AGPD_PATHFIND_NOTUSED;

		Reset();
	}
	void AgpdPathFindBlock::Reset()
	{
		m_iRevision = 0;
		m_iStatus = AGPD_PATHFIND_NOTUSED;
		m_iPathCost = 0;

		m_fCurrentX = 0.0f;
		m_fCurrentY = 0.0f;
	}
};

#endif