/*=====================================================================

	AgsdRefinery.h

=====================================================================*/

#ifndef _AGSD_REFINERY_H_
	#define _AGSD_REFINERY_H_

#include "ApBase.h"
#include "AgpdItemTemplate.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGSMREFINERY_MAX_OPTION		30
#define AGSMREFINERY_MAX_SOCKET		8


// random option
const enum AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN
	{
	AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_OPTION_TID		= 0,
	AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_OPTION_CONT,
	AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_GROUP_ID,
	AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_RATE,
	};


// socket
const enum AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN
	{
	AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN_ID			= 0,
	AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN_MAX_SOCKET,
	AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN_RATE1,
	};




/********************************************/
/*		The Definition of Option Group		*/
/********************************************/
//
class AgsdRefineOptionGroup
	{
	public:
		INT32		m_lID;
		INT32		m_lCount;
		ApSafeArray<INT32, AGSMREFINERY_MAX_OPTION> m_lRates;
		ApSafeArray<INT32, AGSMREFINERY_MAX_OPTION> m_lOptions;
	
	public:
		AgsdRefineOptionGroup()
			{
			Init();
			}

		~AgsdRefineOptionGroup()		{}
		
		void Init()
			{
			m_lID = 0;
			m_lCount = 0;
			m_lRates.MemSetAll();
			m_lOptions.MemSetAll();
			}
	};




/****************************************/
/*		The Definition of Socket		*/
/****************************************/
//
class AgsdRefineSocket
	{
	public:
		INT32		m_lID;
		INT32		m_lCount;
		ApSafeArray<INT32, AGSMREFINERY_MAX_SOCKET> m_Rates;
	
	public:
		AgsdRefineSocket()
			{
			Init();
			}
			
		~AgsdRefineSocket()		{}
		
		void Init()
			{
			m_lID = 0;
			m_lCount = 0;
			m_Rates.MemSetAll();
			}	
	};


#endif