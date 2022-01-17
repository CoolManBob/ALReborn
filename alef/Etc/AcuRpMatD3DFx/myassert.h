//@{ Jaewon 20040813
// copied & modified.
// assertion utilities.
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
 * myassert.h
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 ****************************************************************************/

#ifndef __MYASSERT_H__
#define __MYASSERT_H__

#include <assert.h>

#define BASIC_ASSERTx

#if defined(BASIC_ASSERT)

#define my_assert(x) assert(x)

#else /* BASIC_ASSERT */

#include <crtdbg.h>

#if defined(RWDEBUG)
extern void _rwDebugSendMessage(RwDebugType type, const RwChar *file, const RwInt32 line,
    const RwChar *funcName, const RwChar *message);
#else   /* RWDEBUG */
#define _rwDebugSendMessage(a, b, c, d, e)  /* nop */
#endif  /* RWDEBUG */

#if defined(RWDEBUG)
#define my_assert(_condition)   \
do                              \
{                               \
    if (!(_condition))          \
    {                           \
        _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "Assertion failed:", #_condition);   \
    }                           \
    _ASSERTE((_condition));     \
}                               \
while (0)
#else   /* RWDEBUG */
#define my_assert(x)    /* nop */
#endif  /* RWDEBUG */

#endif /* BASIC_ASSERT */

#if defined(RWDEBUG)
#define TEST_HANDLE_NAME_LENGTH(paramName, funcName)                            \
do{                                                                             \
    if ((strlen(paramName) + 1) > FX_HANDLE_NAME_LENGTH)                        \
    {                                                                           \
        RwChar msg[MAX_MSG_SIZE];                                               \
        _snprintf(msg, MAX_MSG_SIZE, "Name \"%s\"too long. Only first %d characters will be compared against \
stored handle names.", paramName, FX_HANDLE_NAME_LENGTH);                                     \
        _rwDebugSendMessage(rwDEBUGASSERT, __FILE__, __LINE__, funcName, msg);  \
    }                                                                           \
}while(0)                                                                       
#else   /* RWDEBUG */
#define TEST_HANDLE_NAME_LENGTH(paramName, funcName)    /* nop */
#endif  /* RWDEBUG */

#if defined(RWDEBUG)
#define TEST_EFFECT_NAME_LENGTH(name, funcName)                                 \
do{                                                                             \
    if ((strlen(name) + 1) > FX_FILE_NAME_LENGTH)                               \
    {                                                                           \
        RwChar msg[MAX_MSG_SIZE];                                               \
        _snprintf(msg, MAX_MSG_SIZE, "Name \"%s\"too long. Only first %d characters will be stored or used in \
compares.", name, FX_FILE_NAME_LENGTH);                                         \
        _rwDebugSendMessage(rwDEBUGASSERT, __FILE__, __LINE__, funcName, msg);  \
    }                                                                           \
}while(0)                                                                       
#else   /* RWDEBUG */
#define TEST_EFFECT_NAME_LENGTH(name, funcName)    /* nop */
#endif  /* RWDEBUG */


#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* __MYASSERT_H__ */

