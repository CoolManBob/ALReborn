// AuCrypt.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

#include "AuCrypt.h"

//template <class KeyInfo, INT32 lKeyInfoSize>
//AuCryptCell<KeyInfo, lKeyInfoSize>::AuCryptCell() : m_lKeyInfoSize(lKeyInfoSize)
//{
//	// 처음은 bzero 해준다.
//	memset(&m_KeyInfo, 0, sizeof(KeyInfo));
//}
//
//template <class KeyInfo, INT32 lKeyInfoSize>
//AuCryptCell<KeyInfo, lKeyInfoSize>::~AuCryptCell()
//{
//}

AuCryptCell::AuCryptCell()
{
	m_KeyInfo = NULL;
	m_lKeyInfoSize = 0;
}

AuCryptCell::~AuCryptCell()
{
	if(m_KeyInfo)
		delete m_KeyInfo;
}


/////////////////////////////////////////////////////////////////////////
// AuCryptActor

// 기본값은 사용안함 이다.
AuCryptActor::AuCryptActor() : m_eCryptType(AUCRYPT_TYPE_NONE)
{
	m_PublicKey.CleanUp();
	m_PrivateKey.CleanUp();
}

AuCryptActor::~AuCryptActor()
{
}

