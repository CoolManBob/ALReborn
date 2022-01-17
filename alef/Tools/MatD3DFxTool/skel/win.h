
/****************************************************************************
 *
 * win.h
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
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

#if (!defined(_PLATFORM_WIN_H))
#define _PLATFORM_WIN_H


#if (defined(_DEBUG) && defined(RWDEBUG) && defined(RWMEMDEBUG))

#include <tchar.h>
#pragma comment (lib , "advapi32.LIB") /* Registry functions */

#define RsRegOpenMachineKey(result)                             \
MACRO_START                                                     \
{                                                               \
    static const TCHAR  RenderWareKey[] =                       \
        "Software\\Criterion\\RenderWare";                      \
    DWORD               disposition;                            \
    LONG                status =                                \
        RegCreateKeyEx(HKEY_LOCAL_MACHINE, RenderWareKey, 0,    \
                       REG_NONE, REG_OPTION_NON_VOLATILE,       \
                       KEY_READ | KEY_WRITE,                    \
                       NULL, &result, &disposition);            \
                                                                \
    if (status != ERROR_SUCCESS)                                \
    {                                                           \
        result = NULL;                                          \
    }                                                           \
}                                                               \
MACRO_STOP

#define RsRegGetDWordValue(_result, _hKey, _name, _val)                 \
MACRO_START                                                             \
{                                                                       \
    DWORD               _size;                                          \
    DWORD               _type;                                          \
    LONG                _status;                                        \
                                                                        \
    _status =                                                           \
        RegQueryValueEx((_hKey), (_name), 0, &_type, NULL, &_size);     \
    (_result) = ((ERROR_SUCCESS == _status) && (REG_DWORD == _type));   \
                                                                        \
    if ((_result))                                                      \
    {                                                                   \
        _status =                                                       \
            RegQueryValueEx((_hKey), (_name), 0, &_type,                \
                            (BYTE *) (_val), &_size);                   \
        (_result) = (ERROR_SUCCESS == _status);                         \
    }                                                                   \
}                                                                       \
MACRO_STOP

#define RSGETWINREGDWORD(result, match)                 \
MACRO_START                                             \
{                                                       \
    HKEY                hKey;                           \
                                                        \
    RsRegOpenMachineKey(hKey);                          \
    if (hKey)                                           \
    {                                                   \
        RwBool              success;                    \
                                                        \
        RsRegGetDWordValue(success, hKey, match,        \
                               &result);                \
                                                        \
        RegCloseKey(hKey);                              \
    }                                                   \
}                                                       \
MACRO_STOP


#define RSREGSETBREAKALLOC(_name)                       \
MACRO_START                                             \
{                                                       \
    char _message[256];                                 \
    long _lBreakAlloc = -1;                             \
                                                        \
    RSGETWINREGDWORD(_lBreakAlloc, _name);              \
                                                        \
     _CrtSetBreakAlloc(_lBreakAlloc);                   \
                                                        \
    _snprintf(_message, sizeof(_message),               \
              "%s(%d): RSCRTSETBREAKALLOC(%ld)\n",      \
              __FILE__, __LINE__,                       \
              _lBreakAlloc);                            \
    OutputDebugString(_message);                        \
                                                        \
}                                                       \
MACRO_STOP

#endif /* (defined(_DEBUG) && defined(RWDEBUG) && defined(RWMEMDEBUG)) */

#if (!defined(RSREGSETBREAKALLOC))
#define RSREGSETBREAKALLOC(_name) /* No op */
#endif /* (!defined(RSREGSETBREAKALLOC)) */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern HWND
psWindowGetHandle(void);

extern LRESULT      CALLBACK
MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */


#endif /* (!defined(_PLATFORM_WIN_H)) */
