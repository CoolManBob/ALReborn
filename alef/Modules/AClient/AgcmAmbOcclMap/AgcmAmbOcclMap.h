// -----------------------------------------------------------------------------
//                                                   _      ____            _ __  __                 _     
//     /\                            /\             | |    / __ \          | |  \/  |               | |    
//    /  \    __ _  ___ _ __ ___    /  \   _ __ ___ | |__ | |  | | ___  ___| | \  / | __ _ _ __     | |__  
//   / /\ \  / _` |/ __| '_ ` _ \  / /\ \ | '_ ` _ \| '_ \| |  | |/ __|/ __| | |\/| |/ _` | '_ \    | '_ \ 
//  / ____ \| (_| | (__| | | | | |/ ____ \| | | | | | |_) | |__| | (__| (__| | |  | | (_| | |_) | _ | | | |
// /_/    \_\\__, |\___|_| |_| |_/_/    \_\_| |_| |_|_.__/ \____/ \___|\___|_|_|  |_|\__,_| .__/ (_)|_| |_|
//            __/ |                                                                       | |              
//           |___/                                                                        |_|              
//
// Archlord object extension for saving ambient occlusion map infos
//
// -----------------------------------------------------------------------------
// Originally created on 12/21/2004 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2004, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCMAMBOCCLMAP
#define _H_AGCMAMBOCCLMAP

#include "AgcModule.h"
#include "ApmObject.h"
#include "ApdObject.h"
#include "AgcmObject.h"
#include "AgcdObject.h"
#include "AcuRtAmbOcclMap.h"
//@{ Jaewon 20050721
// ;)
#include "ApMemoryPool.h"
//@} Jaewon

#define AGCMAMBOCCLMAP_INI_NAME_ATOMIC_COUNT	"AmbOcclMapAtomicCount"
#define AGCMAMBOCCLMAP_INI_NAME_ATOMIC_ID		"AmbOcclMapAtomicId"
#define AGCMAMBOCCLMAP_INI_NAME_OBJECT_FLAGS	"AmbOcclMapObjectFlags"
#define AGCMAMBOCCLMAP_INI_NAME_MAP_SIZE		"AmbOcclMapSize"
#define AGCMAMBOCCLMAP_INI_NAME_MAP_NAME		"AmbOcclMapName"

// -----------------------------------------------------------------------------

class AgcmAmbOcclMap : public AgcModule
{
public:
	//@{ Jaewon 20050721
	// Introduce 'AtomicData'.
	struct AtomicData
	{
		// array of atomic id
		int atomicId_;
		// See RtAmbOcclMapObjectFlags.
		RwUInt32 flags_;
		// Saved per-atomic in case ambient occlusion maps are missing on load and
		// so blank ambient occlusion maps of the appropriate size have to be created.
		unsigned int mapSize_;
		// The name of this atomic's ambient occlusion map
		char mapName_[32];

		// The next pointer for listing
		AtomicData *next_;

	};
	struct ObjectData
	{
		// number of atomics which belong to this object
		unsigned int atomicCount_;

		AtomicData *atomicData_;
	};
	//@} Jaewon
public:
	// Construction/Destruction
	AgcmAmbOcclMap();
	virtual	~AgcmAmbOcclMap();

	// Operators

	// Accessors

	// Interface methods
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();

	// get its ObjectData form a ApdObject.
	ObjectData* getObjectData(ApdObject *pApdObject);

	//@{ Jaewon 20050721
	// Introduce 'AtomicData' & the memory pool.
	AtomicData *allocAtomicData(RwUInt32 count);
	void freeAtomicData(AtomicData *atomicData);
	//@} Jaewon

	// Stream read/write callbacks
	static BOOL objectStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL objectStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	// Object data destructor/constructor
	static BOOL	objectConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	objectDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	// Object initialization callbacks
	static BOOL initObjectCB(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	// Data members
	// Pointers to the related modules
	ApmObject *pApmObject_;
	AgcmObject *pAgcmObject_;
	// Index of attached-to-ApmObject data
	short objectAttachIndex_;
	
	//@{ Jaewon 20050721
	// Memory pool for ObjectData
	ApMemoryPool objectDataMemPool_;
	//@} Jaewon

	// Implementation methods
	// search clump for an atomic of the specified id.
	RpAtomic *searchAtomicById(ApdObject *pApdObject, int id);
};

#endif // _H_AGCMAMBOCCLMAP
// -----------------------------------------------------------------------------
// AgcmAmbOcclMap.h - End of file
// -----------------------------------------------------------------------------

