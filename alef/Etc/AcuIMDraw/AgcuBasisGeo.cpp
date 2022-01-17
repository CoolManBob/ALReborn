// AgcuBasisGeo.cpp
// -----------------------------------------------------------------------------
//                            ____            _      _____                                 
//     /\                    |  _ \          (_)    / ____|                                
//    /  \    __ _  ___ _   _| |_) | __ _ ___ _ ___| |  __  ___  ___       ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | |  _ < / _` / __| / __| | |_ |/ _ \/ _ \     / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | |_) | (_| \__ \ \__ \ |__| |  __/ (_) | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|____/ \__,_|___/_|___/\_____|\___|\___/ (_) \___| .__/| .__/ 
//            __/ |                                                           | |   | |    
//           |___/                                                            |_|   |_|    
//
// -----------------------------------------------------------------------------
// Originally created on 02/03/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "MagDebug.h"
#include "AgcuBasisGeo.h"

AGCUGEOUD_BEGIN;

// =============================================================================
//* stGeoInfo
// =============================================================================
LPCSTR stGeoInfo::SZTYPE[stGeoInfo::E_NUM] = {
	"BOX",
	"SPHERE",
	"CONE",
	"CYLINDER",
};
stGeoInfo::stGeoInfo(etype type, LPCSTR szName)
: m_etype(type)
{
	memset(m_szName, 0, sizeof(m_szName[0])*NAME_LEN);
	if(szName)
		strncpy(m_szName, szName, NAME_LEN-1);
}
LPCSTR stGeoInfo::GetTypeName()const
{
	return SZTYPE[m_etype];
}
LPCSTR stGeoInfo::GetName()const
{
	return m_szName;
}

AGCUGEOUD_END;