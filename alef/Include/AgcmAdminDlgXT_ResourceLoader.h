// AgcmAdminDlgXT_ResourceLoader.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 05. 20.
//
// Admin Dialog 에서 보여줄 필요가 있는
// Texture 를 읽어서 GDI+ 의 Image 객체로 관리한다.
// 생성한 Image 객체는 Pool 에 집어넣고 사용한다.

class CTextureItem
{
public:
	CHAR m_szTextureName[AGPMADMIN_MAX_PATH+1];
	Gdiplus::Image* m_pcsImage;

public:
	CTextureItem();
	virtual ~CTextureItem();

	BOOL CheckTextureName(LPCTSTR szTextureName);
	Gdiplus::Image* GetImage();
};

class AgcmAdminDlgXT_ResourceLoader
{
private:
	ApMemoryPool m_csMemoryPool;
	list<CTextureItem*> m_listTextureItem;

public:
	AgcmAdminDlgXT_ResourceLoader();
	virtual ~AgcmAdminDlgXT_ResourceLoader();

	Gdiplus::Image* GetImage(LPCTSTR szTextureName);


	CTextureItem* GetTextureItem(LPCTSTR szTextureName);
	CTextureItem* MakeNewTextureItem(LPCTSTR szTextureName);

	BOOL ClearTextureItemList();
};

