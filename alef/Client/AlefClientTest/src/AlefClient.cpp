
/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 2001 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * main.c
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: 
 * Reviewed by: 
 *
 * Purpose: Starting point for any new RW demo using the demo skeleton.
 *
 ****************************************************************************/
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include "skeleton.h"
#include "menu.h"
//#include "events.h"
#include "camera.h"

#include "AgcEngine.h"

#ifdef RWMETRICS
//#include "metrics.h"
#endif


/*
 *****************************************************************************
 */

// 마고자
// 엔진 클래스 메모리 할당.

/*
 *****************************************************************************
 */

//RsEventStatus
//AppEventHandler(RsEvent event, void *param)
//{
//	// 엔진 클래스로 모든 메시지 포워딩함.
//	return g_pEngine->OnMessageSink( event , param );
//}

/*
 *****************************************************************************
 */
