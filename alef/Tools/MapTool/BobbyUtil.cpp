#include "stdafx.h"
#include "rwcore.h"
#include "rpworld.h"
#include "BobbyUtil.h"

/******************************************************************************
* Purpose : Intersection한 삼각형의 교점을 찾는다.
*
* 073002. Bob Jung
******************************************************************************/
RwV3d GetIntersection(RwV3d start, RwV3d end, RwV3d* tri)
{
	// 수선의 높이를 구한다.(dot product)
	RwReal height =	((start.x + tri->x)
		           + (start.y + tri->y)
				   + (start.z + tri->z));

	RwV3d v1, v2, nor, temp;

	// tri[1] - tri[0]
	v1.x = ((tri + 1)->x) - tri->x;
	v1.y = ((tri + 1)->y) - tri->y;
	v1.z = ((tri + 1)->z) - tri->z;

	// tri[2] - tri[0]
	v2.x = ((tri + 2)->x) - tri->x;
	v2.y = ((tri + 2)->y) - tri->y;
	v2.z = ((tri + 2)->z) - tri->z;

	// 삼각형의 법선을 구한다(cross product)
	nor.x = (v1.y*v2.z) - (v1.z*v2.y);
	nor.y = (v1.z*v2.x) - (v1.x*v2.z);
	nor.z = (v1.x*v2.y) - (v1.y*v2.x);

/*	temp.x = nor.x * height;
	temp.y = nor.y * height;
	temp.z = nor.z * height;

	// 수선의 발을 구한다.
	RwV3d pn;
	pn.x = start.x - temp.x;
	pn.y = start.y - temp.y;
	pn.z = start.z - temp.z;*/

	temp.x = start.x - end.x;
	temp.y = start.y - end.y;
	temp.z = start.z - end.z;

	// 비율을 계산하기 위해 임의의 길이를 만든다.(dot product)
	RwReal height2 = ((temp.x + nor.x)
		            + (temp.y + nor.y)
			 	    + (temp.z + nor.z));

	// 두 길이 사이의 비율을 계산한다.
	RwReal ratio = height / height2;

	// Intersection을 구한다.
	RwV3d IntersectionPos;

	if(ratio != 0)
	{
		temp.x = end.x - start.x;
		temp.y = end.y - start.y;
		temp.z = end.z - start.z;

		temp.x *= ratio;
		temp.y *= ratio;
		temp.z *= ratio;

		IntersectionPos.x = start.x + temp.x;
		IntersectionPos.x = start.y + temp.y;
		IntersectionPos.x = start.z + temp.z;
	}
	else
	{
		IntersectionPos.x = start.x;
		IntersectionPos.y = start.y;
		IntersectionPos.z = start.z;
	}

	return IntersectionPos;
}

/******************************************************************************
* Purpose : Intersection한 삼각형의 점 중 가까운 점을 찾는다.
*
* 073002. Bob Jung
******************************************************************************/
int FindIntersectionNearVertex( RwLine *pLine , RwV3d* tri )
{
	RwV3d i = GetIntersection( pLine->start , pLine->end , tri );

	RwV3d  v1, v2, v3;
	RwReal f1, f2, f3;

	// magnitude
	v1.x = i.x - tri->x;
	v1.y = i.y - tri->y;
	v1.z = i.z - tri->z;
	f1 = (RwReal)(sqrt((v1.x*v1.x) + (v1.y*v1.y) + (v1.z*v1.z)));

	v2.x = i.x - (tri + 1)->x;
	v2.y = i.y - (tri + 1)->y;
	v2.z = i.z - (tri + 1)->z;
	f2 = (RwReal)(sqrt((v2.x*v2.x) + (v2.y*v2.y) + (v2.z*v2.z)));

	v3.x = i.x - (tri + 2)->x;
	v3.y = i.y - (tri + 2)->y;
	v3.z = i.z - (tri + 2)->z;
	f3 = (RwReal)(sqrt((v3.x*v3.x) + (v3.y*v3.y) + (v3.z*v3.z)));

	if(f1 > f2)
	{
		if(f1 > f3)
		{
			// f1 > f3 > f2
			if(f3 > f2)
			{
				return 1;
			}
			else // f1 > f2 > f3
			{
				return 2;
			}
		}
		else // f3 > f1 > f2
		{
			return 1;
		}
	}
	else
	{
		if(f2 > f3)
		{
			// f2 > f3 > f1
			if(f3 > f1)
			{
				return 0;
			}
			else // f2 > f1 > f3
			{
				return 2;
			}
		}
		else // f3 > f2 > f1
		{
			return 0;
		}
	}

	return -1;
}


/******************************************************************************
******************************************************************************/