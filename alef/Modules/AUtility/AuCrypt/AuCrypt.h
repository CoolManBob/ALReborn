// AuCrypt.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

//
// AuCryptCell 은 BYTE 데이터의 Encrypt/Decrypt 인터페이스를 제공한다.
// 실제 알고리즘은 AuCryptManager 에 있다.
//
// AuCryptActor 은 Encrypt/Decrypt 가 실제 필요한 곳에서 인스턴스화 된다.
//

#ifndef _AUCRYPT_H_
#define _AUCRYPT_H_

#include "ApBase.h"
#include "AuCryptAlgorithm.h"

// 암호화를 수행할 가장 기본적인 클래스
// Remove Template. 2006.06.13. steeple
//template <class KeyInfo, INT32 lKeyInfoSize = sizeof(KeyInfo)>

class AuCryptCell
{
public:
	typedef BASE_CTX*		KeyInfo;	// KeyInfo is Pointer Type

private:
	KeyInfo m_KeyInfo;

	INT32 m_lKeyInfoSize;

public:
	AuCryptCell();
	virtual ~AuCryptCell();

	void SetKey(KeyInfo key) { m_KeyInfo = key; }

	void SetKeyInfoSize(INT32 lSize) { m_lKeyInfoSize = lSize; }
	INT32 GetKeyInfoSize() { return m_lKeyInfoSize; }

	//KeyInfo& GetKeyRef() { return m_KeyInfo; }
	//KeyInfo* GetKeyPtr() { return &m_KeyInfo; }

	KeyInfo GetKeyPtr() { return m_KeyInfo; }
	KeyInfo GetKey()	{ return m_KeyInfo; }	// Enable by KeyInfo is Pointer Type

	void CleanUp() { if(m_KeyInfo) m_KeyInfo->init(); }
};

// public, private 를 가지고 있고, 현재 상태에 따라 적절한 일을 함.
class AuCryptActor
{
public:
//#ifdef _AREA_CHINA_
//	typedef AuCryptCell<DYNCODE_CTX>		PublicKeyInfo;
//	typedef AuCryptCell<DYNCODE_CTX>		PrivateKeyInfo;		// 일단 Private 도 BlowFish
//#else
//	typedef AuCryptCell<BLOWFISH_CTX>		PublicKeyInfo;
//	typedef AuCryptCell<BLOWFISH_CTX>		PrivateKeyInfo;		// 일단 Private 도 BlowFish
//#endif
//
private:
	eAuCryptType m_eCryptType;

	AuCryptCell m_PublicKey;
	AuCryptCell m_PrivateKey;			

public:
	AuCryptActor();
	virtual ~AuCryptActor();

	// 아래는 기본적인 타입이 이렇다는 것이고, 실제로는 public/private 모두 사용 가능하다.
	// 처음 설계시에는 하나만 되게 하려했으나 중간에 바꿨음. None 이냐 아니냐만 체크하면 된다.
	void SetCryptType(eAuCryptType eType) { m_eCryptType = eType; }
	eAuCryptType GetCryptType() { return m_eCryptType; }

	AuCryptCell& GetPublicKeyRef() { return m_PublicKey; }
	AuCryptCell* GetPublicKeyPtr() { return &m_PublicKey; }

	AuCryptCell& GetPrivateKeyRef() { return m_PrivateKey; }
	AuCryptCell* GetPrivateKeyPtr() { return &m_PrivateKey; }

	void CleanUpPublic() { m_PublicKey.CleanUp(); }
	void CleanUpPrivate() { m_PrivateKey.CleanUp(); }

	void Initialize() { CleanUpPublic(); CleanUpPrivate(); SetCryptType(AUCRYPT_TYPE_NONE); }
};

#endif //_AUCrypt_H_
