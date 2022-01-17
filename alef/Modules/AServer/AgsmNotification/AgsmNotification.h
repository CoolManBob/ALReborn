/******************************************************************************
Module:  AgsmNotification.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 28
******************************************************************************/

#if !defined(__AGSMNOTIFICATION_H__)
#define __AGSMNOTIFICATION_H__


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmNotificationD" )
#else
#pragma comment ( lib , "AgsmNotification" )
#endif
#endif


class AgsmNotification : public AgsModule {
};

#endif //__AGSMNOTIFICATION_H__