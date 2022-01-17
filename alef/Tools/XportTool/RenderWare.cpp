#include "RenderWare.h"
#include <cassert>
#include <rwcore.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rttiff.h>
#include <rtcharse.h>
#include <rpworld.h>
#include <rtquat.h>
#include <rphanim.h>
#include <rpskin.h>
#include <rpusrdat.h>
#include <rtintsec.h>
#include <rpcollis.h>
#include <rtpick.h>
#include <rtanim.h>
#include <rphanim.h>
#include <rpspline.h>
#include <rplodatm.h>
#include <rpmatfx.h>
#include <rpmorph.h>
#include <rtdict.h>
#include <rpuvanim.h>

#include "AcuRpUVAnimData.h"
#include "AcuRpMatD3DFx.h"

#include <rtfsyst.h>

#include "AcuRpMTexture.h"

#define MAX_NB_FILES_PER_FS (20)   

bool RenderWare::initialize(HWND hWnd)
{
	if( !RwEngineInit( NULL, NULL, 0, (4 << 20) ) )
		return false;

	if( !attachPlugins() )				goto FAIL;

	RwEngineOpenParams openParams;
	openParams.displayID = hWnd;
	if( !RwEngineOpen(&openParams) )	goto FAIL;
	if( !RwEngineStart() )				goto FAIL;
	if( !RtFSManagerOpen(RTFSMAN_UNLIMITED_NUM_FS) || !installFileSystem() )	goto FAIL;
	if( !registerImageLoaders() )		goto FAIL;

	return true;

FAIL:
	RwEngineStop();
	RwEngineClose();
	RwEngineTerm();
	return false;
}

void RenderWare::destroy()
{
	RtFileSystem* fs = RtFSManagerGetFileSystemFromName("unc");
	if( fs )
		RtFSManagerUnregister(fs);
	RtFSManagerClose();

	RwEngineStop();
	RwEngineClose();
	RwEngineTerm();
}

bool RenderWare::attachPlugins()
{
	if( !RpWorldPluginAttach() )		return false;
	if( !RpHAnimPluginAttach() )		return false;
	if( !RpSkinPluginAttach() )			return false;
	if( !RpUserDataPluginAttach() )		return false;
	if( !RpCollisionPluginAttach() )	return false;
	if( !RtAnimInitialize() )			return false;
	if( !RpSplinePluginAttach() )		return false;
	if( !RpMTexturePluginAttach() )		return false;
	if( !RpMorphPluginAttach() )		return false;
	if( !RpLODAtomicPluginAttach() )	return false;
	if( !RpMatFXPluginAttach() )		return false;
	if( !RpUVAnimPluginAttach() )		return false;
	if( !RpUVAnimDataPluginAttach() )	return false;
	if( !RpMaterialD3DFxPluginAttach() )return false;

	return true;
}

bool RenderWare::installFileSystem()
{
	RwChar szDir[_MAX_PATH];
	RwUInt32 retValue = GetCurrentDirectory( _MAX_PATH, szDir );
	if( !retValue || retValue > _MAX_PATH)	return false;

	strcat(szDir, "\\");

	RtFileSystem* unc = RtWinFSystemInit(MAX_NB_FILES_PER_FS, szDir, "unc");
	if( !unc )	return false;
	if( !RtFSManagerRegister(unc) )	return false;

	CharUpper(szDir);

	RwUInt32 drivesMask = GetLogicalDrives();
	RwInt32 drive;
	for(drive = 2, drivesMask >>= 1; drivesMask != 0; drive++, drivesMask >>= 1)
	{
		if(drivesMask & 0x01)
		{
			RwChar deviceName[4] = { drive + 'A' - 1, ':', '\\', '\0' };
			RwInt32 ret = GetDriveType( deviceName );
			if( DRIVE_UNKNOWN == ret || DRIVE_NO_ROOT_DIR == ret )
				continue;

			deviceName[2] = '\0';		/* Fix device name */

			RwChar fsName[2];
			fsName[0] = deviceName[0];
			fsName[1] = '\0';
			RtFileSystem* wfs = RtWinFSystemInit( MAX_NB_FILES_PER_FS, deviceName, fsName );
			if( !wfs )		return false;
			if( !RtFSManagerRegister(wfs) )		return false;

			/* Set the unc file system as default if we have a unc path */
			if(szDir[1] != ':')
				RtFSManagerSetDefaultFileSystem(unc);
			else if(deviceName[0] == szDir[0])
				RtFSManagerSetDefaultFileSystem(wfs);
		}
	}

	return true;
}

bool RenderWare::registerImageLoaders()
{
	if( !RwImageRegisterImageFormat( RWSTRING("bmp"), RtBMPImageRead, 0 ) )		return false;
	if( !RwImageRegisterImageFormat( RWSTRING("png"), RtPNGImageRead, 0 ) )		return false;
	if( !RwImageRegisterImageFormat( RWSTRING("tif"), RtTIFFImageRead, 0 ) )	return false;

	return true;
}
