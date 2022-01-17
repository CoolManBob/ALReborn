#ifndef __CLASS_RENDER_DEVICE_RENDERWARE_H__
#define __CLASS_RENDER_DEVICE_RENDERWARE_H__




#include "CRenderDevice.h"


#define RW_MAX_SUBSYSTEM_COUNT				16


class CRenderDevice_Renderware : public CRenderDevice
{
private :
	int										m_nCurrentSystemIndex;
	int										m_nCurrentVideoMode;

public :
	CRenderDevice_Renderware( void );
	virtual ~CRenderDevice_Renderware( void );

public :
	virtual BOOL	OnCreate				( stRenderDeivceSettings* pDeviceSettings );
	virtual BOOL	OnDestroy				( void );
	virtual BOOL	OnSceneStart			( void );
	virtual BOOL	OnSceneEnd				( void );
	virtual BOOL	OnSetDefaultBlendMode	( void );

private :
	BOOL			_SetMemoryAllocators	( void );
	BOOL			_AttachPlugins			( void );
	BOOL			_MakeEngineOpenParams	( void* pParam );
	BOOL			_SelectVideoDevice		( void );
	int				_GetSuitableVideoMode	( void );
	BOOL			_RegisterImageLoader	( void );
	BOOL			_OpenFileSystem			( void );
	BOOL			_InstallFileSystem		( void );
};




#endif