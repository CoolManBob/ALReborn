// AgcmAdminDlgXT_ResourceLoader.pp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 05. 20.

#include "StdAfx.h"
#include "AcuTexture.h"
#include "AuMD5Encrypt.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>



#define	FILE_MAX_SIZE	(1024 * 1024)

CTextureItem::CTextureItem()
{
	memset(m_szTextureName, 0, AGPMADMIN_MAX_PATH+1);
	m_pcsImage = NULL;
}

CTextureItem::~CTextureItem()
{
	if(m_pcsImage)
		delete m_pcsImage;
}

BOOL CTextureItem::CheckTextureName(LPCTSTR szTextureName)
{
	if(!szTextureName)
		return FALSE;

	if(strcmp(m_szTextureName, szTextureName) == 0)
		return TRUE;
	else
		return FALSE;
}

Gdiplus::Image* CTextureItem::GetImage()
{
	return m_pcsImage;
}






//////////////////////////////////////////////////////////////////////////
// Resource Loader
AgcmAdminDlgXT_ResourceLoader::AgcmAdminDlgXT_ResourceLoader()
{
	m_csMemoryPool.Initialize(FILE_MAX_SIZE, 4);
}

AgcmAdminDlgXT_ResourceLoader::~AgcmAdminDlgXT_ResourceLoader()
{
	ClearTextureItemList();
}

Gdiplus::Image* AgcmAdminDlgXT_ResourceLoader::GetImage(LPCTSTR szTextureName)
{
	if(!szTextureName)
		return NULL;

	CTextureItem* pTextureItem = GetTextureItem(szTextureName);
	if(!pTextureItem)
		pTextureItem = MakeNewTextureItem(szTextureName);

	if(!pTextureItem)
		return NULL;

	return pTextureItem->GetImage();
}

CTextureItem* AgcmAdminDlgXT_ResourceLoader::GetTextureItem(LPCTSTR szTextureName)
{
	if(!szTextureName)
		return NULL;

	list<CTextureItem*>::iterator iterData = m_listTextureItem.begin();
	while(iterData != m_listTextureItem.end())
	{
		if((*iterData)->CheckTextureName(szTextureName))
			return *iterData;

		iterData++;
	}

	return NULL;
}

CTextureItem* AgcmAdminDlgXT_ResourceLoader::MakeNewTextureItem(LPCTSTR szTextureName)
{
	if(!szTextureName)
		return NULL;

	// 이미 리스트에 있다면 그놈을 리턴한다.
	CTextureItem* pcsTextureItem = GetTextureItem(szTextureName);
	if(pcsTextureItem)
		return pcsTextureItem;

	// 없다면 새로 만들어서 넣어준다.
	// 여기서 부터 중요하다.
	// 암호화 되어 있기 때문에 풀어서 Texture 파일을 생성한 후 Image 객체를 만들어서 넣은 다음 지운다.

	CHAR szEncryptedFileName[AGPMADMIN_MAX_PATH+1];
	CHAR szImageFileName[AGPMADMIN_MAX_PATH+1];

	sprintf(szEncryptedFileName, "%s.%s", szTextureName, "tx2");
	sprintf(szImageFileName, "%s.%s", szTextureName, "png");

	// 암호화된 파일을 연다.
	INT32 hEncryptedFile = _open(szEncryptedFileName, _O_RDONLY | _O_BINARY, _S_IREAD);
	if(hEncryptedFile == -1)
		return NULL;

	unsigned char* pBuffer = NULL;
	INT32 lFileLength = _filelength(hEncryptedFile);

	// File Size 가 0 이거나 너무 크면 나간다.
	if(lFileLength == 0 || lFileLength > FILE_MAX_SIZE)
	{
		_close(hEncryptedFile);
		return NULL;
	}

	// 암호화된 파일을 읽는다.
	pBuffer = (unsigned char*)m_csMemoryPool.Alloc();
	if(_read(hEncryptedFile, pBuffer, lFileLength) != lFileLength)
	{
		_close(hEncryptedFile);
		m_csMemoryPool.Free(pBuffer);
		return NULL;
	}

	// 일단 파일을 닫고
	_close(hEncryptedFile);

	AuMD5Encrypt csMD5;
	// 암호화를 푼다.
	if(!csMD5.DecryptString(ACU_TEXTURE_MD5_HASH_STRING, (char*)pBuffer, lFileLength))
	{
		m_csMemoryPool.Free(pBuffer);
		return NULL;
	}

	// Texture Image 파일을 만든다.
	BOOL bExist = FALSE;
	INT32 hImageFile = _open(szImageFileName, _O_BINARY | _O_WRONLY | _O_CREAT, _S_IREAD | _S_IWRITE);
	if(hImageFile == -1)
	{
		if(errno != EEXIST)
		{
			_close(hImageFile);
			_unlink(szImageFileName);
			m_csMemoryPool.Free(pBuffer);
			return NULL;
		}
		
		bExist = TRUE;
	}

	// Image 파일이 없을땐
	if(!bExist)
	{
		// 쓴다.
		if(_write(hImageFile, pBuffer, lFileLength) != lFileLength)
		{
			_close(hImageFile);
			_unlink(szImageFileName);
			m_csMemoryPool.Free(pBuffer);
			return NULL;
		}
	}

	// 닫는다.
	_close(hImageFile);

	// 여기까지 오면 암호화 풀고 Texture Image 를 임시로 만든 상황이다.
	pcsTextureItem = new CTextureItem;
	strcpy(pcsTextureItem->m_szTextureName, szTextureName);

	INT32 lImageFilePathLength = strlen(szImageFileName);
	WCHAR* wszFullPath = new WCHAR[lImageFilePathLength + 1];
	memset(wszFullPath, 0, sizeof(WCHAR) * (lImageFilePathLength + 1));
	MultiByteToWideChar(CP_ACP, 0, szImageFileName, -1, wszFullPath, lImageFilePathLength);
	
	pcsTextureItem->m_pcsImage = new Gdiplus::Image(wszFullPath);
	
	// 리스트에 넣는다.
	m_listTextureItem.push_back(pcsTextureItem);

	// Wide String 해제한다.
	delete [] wszFullPath;

	// Memory Buffer 해제한다.
	m_csMemoryPool.Free(pBuffer);

	// 임시로 만든 Texture Image 파일을 지운다.
	if(!bExist)
		_unlink(szImageFileName);

	return pcsTextureItem;
}

BOOL AgcmAdminDlgXT_ResourceLoader::ClearTextureItemList()
{
	list<CTextureItem*>::iterator iterData = m_listTextureItem.begin();
	while(iterData != m_listTextureItem.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listTextureItem.clear();

	return TRUE;
}