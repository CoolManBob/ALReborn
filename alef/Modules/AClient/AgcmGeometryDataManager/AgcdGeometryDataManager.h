#ifndef __AGCD_GEOMETRY_MANAGER_H__
#define __AGCD_GEOMETRY_MANAGER_H__

#include "ApDefine.h"

typedef AuBLOCKING AcuGeometry;

class AgcdGeometryData
{
public:
	AcuGeometry				m_stGeom;
	AgcdGeometryData		*m_pcsNext;

	AgcdGeometryData()
	{
		m_pcsNext		= NULL;
		memset(&m_stGeom, 0, sizeof(AcuGeometry));
	}
};

class AgcdGeometryDataLink
{
public:
	AgcdGeometryData		*m_pcsHead;

	AgcdGeometryDataLink()
	{
		m_pcsHead		= NULL;
	}
};

class AgcdGeometryFrame
{
public:
	AgcdGeometryDataLink	*m_acsLink;

	AgcdGeometryFrame()
	{
		m_acsLink		= NULL;
	}
};

#endif // __AGCD_GEOMETRY_MANAGER_H__