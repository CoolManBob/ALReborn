//@{ Jaewon 20040813
// copied & modified.
// a renderware rendering pipeline which uses a d3d fx file.
//@} Jaewon

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
 * Copyright (c) 2004 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * pipeline.h
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 ****************************************************************************/

#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwBool
EffectPipelineOpen(void);

extern void
EffectPipelineClose(void);

extern RxPipeline *
EffectGetPipeline(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* __PIPELINE_H__ */

