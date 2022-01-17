// XTPImageManager.h : interface for the CXTPImageManager class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPIMAGEMANAGER_H__)
#define __XTPIMAGEMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//-----------------------------------------------------------------------
// Summary:
//     Image state enumerators.
// Example:
// <code>
// // Set Icons
// pCommandBars->GetImageManager()->SetIcons(IDR_MAINFRAME, xtpImageNormal);
// pCommandBars->GetImageManager()->SetIcons(IDR_MAINFRAME, IDR_MAINFRAME_HOT, xtpImageHot);
// </code>
// See Also: CXTPImageManager::SetIcon, CXTPImageManager::SetIcons
//
// <KEYWORDS xtpImageNormal, xtpImageDisabled, xtpImageHot, xtpImageChecked>
//-----------------------------------------------------------------------
enum XTPImageState
{
	xtpImageNormal,     // Normal image.
	xtpImageDisabled,   // Disabled image.
	xtpImageHot,        // Hot image.
	xtpImageChecked,    // Checked(selected) image.
	xtpImagePressed     // Pressed image.
};


class CXTPImageManager;
class CXTPImageManagerIconSet;
class CXTPImageManagerImageList;
class CXTPPropExchange;


//===========================================================================
// Summary:
//     CXTPImageManagerIconHandle is HICON wrapper class.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerIconHandle
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManagerIconHandle object
	// Parameters:
	//     hIcon - Icon handle
	//     hBitmap - Bitmap handle
	//     hHandle - CXTPImageManagerIconHandle reference
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle();
	CXTPImageManagerIconHandle(HICON hIcon); // <combine CXTPImageManagerIconHandle::CXTPImageManagerIconHandle>
	CXTPImageManagerIconHandle(HBITMAP hBitmap); // <combine CXTPImageManagerIconHandle::CXTPImageManagerIconHandle>
	CXTPImageManagerIconHandle(const CXTPImageManagerIconHandle& hHandle); // <combine CXTPImageManagerIconHandle::CXTPImageManagerIconHandle>

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerIconHandle object, handles cleanup and deallocation
	//-------------------------------------------------------------------------
	~CXTPImageManagerIconHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator returns a HICON object whose value is copied from this CXTPImageManagerIconHandle object.
	// Returns:
	//     HICON handle.
	//-----------------------------------------------------------------------
	operator HICON() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies data from an HBITMAP or CXTPImageManagerIconHandle object
	// Parameters:
	//     handle - CXTPImageManagerIconHandle object
	//     hBitmap - Alpha bitmap handler
	//-----------------------------------------------------------------------
	void CopyHandle(HBITMAP hBitmap);
	void CopyHandle(CXTPImageManagerIconHandle& handle); // <combine CXTPImageManagerIconHandle::CopyHandle@HBITMAP>

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the image has alpha channel.
	// Returns:
	//     TRUE if the image has alpha channel; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL IsAlpha() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if class has no icon.
	// Returns:
	//     TRUE if m_hIcon member is NULL.
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves bitmap handle
	// Returns:
	//     32bit alpha bitmap
	//-----------------------------------------------------------------------
	HBITMAP GetBitmap() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon handle.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	HICON GetIcon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the size of the icon.
	// Returns:
	//     Size of the icon.
	//-----------------------------------------------------------------------
	CSize GetExtent() const;

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Release bitmap and icon handlers
	//-------------------------------------------------------------------------
	void Clear();

	//-------------------------------------------------------------------------
	// Summary:
	//     Creates an icon from resource bits describing the icon.
	// Parameters:
	//     lpszResourceName - Resource name identifier
	//     szIcon - Specifies the desired width and height, in pixels, of the icon.
	//     bGroupResource - TRUE to check if RT_GROUP_ICON resource exists.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-------------------------------------------------------------------------
	BOOL CreateIconFromResource(LPCTSTR lpszResourceName, CSize szIcon, BOOL bGroupResource = TRUE);
	BOOL CreateIconFromResource(HINSTANCE hInst, LPCTSTR lpszResourceName, CSize szIcon, BOOL bGroupResource); // <combine CXTPImageManagerIconHandle::CreateIconFromResource@LPCTSTR@CSize@BOOL>

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator assigns the icon handler to a CXTPImageManagerIconHandle object.
	// Parameters:
	//     hIcon - source icon handler
	//-----------------------------------------------------------------------
	const CXTPImageManagerIconHandle& operator=(const HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator assigns the alpha bitmap handler to a CXTPImageManagerIconHandle object.
	// Parameters:
	//     hBitmap - Alpha bitmap handler
	const CXTPImageManagerIconHandle& operator=(const HBITMAP hBitmap);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Create alpha bitmap with valid alpha layer.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL PreMultiply();

private:
	const CXTPImageManagerIconHandle& operator=(const CXTPImageManagerIconHandle&);


protected:
	HICON   m_hIcon;          // The underlying HICON handler for this CXTPImageManagerIconHandle object.
	HBITMAP m_hBitmap;        // The underlying Alpha bitmap handler for this CXTPImageManagerIconHandle object.
	HBITMAP m_hAlphaBits;     // Valid alpha bitmap bits
	BOOL    m_bClearHandles;  // TRUE if object must destroy handlers.

private:
	friend class CXTPImageManagerIcon;
};

//===========================================================================
// Summary:
//     Icons holder class.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerIcon : public CXTPCmdTarget
{
private:
	struct ICONDIRENTRY;
	struct ICONDIRHEADER;
	struct ICONIMAGE;
	struct GRPICONDIRENTRY;
	struct GRPICONDIR;

	//-----------------------------------------------------------------------
	// Summary:
	//     Scale to specified Width
	// Parameters:
	//     nWidth - Width to be scaled.
	// Returns:
	//     New CXTPImageManagerIcon object with specified width icons.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* Scale(int nWidth) const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManagerIcon object.
	// Parameters:
	//     nID - Icon identifier.
	//     nWidth - Initial width for the icon.
	//     nHeight - Initial height for the icon.
	//     pIconSet - Parent icon set icon will belong.
	//     pImageList - Parent icon set icon will belong.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon(UINT nID, int nWidth, int nHeight, CXTPImageManagerIconSet* pIconSet = NULL);
	CXTPImageManagerIcon(UINT nID, int nWidth, int nHeight, CXTPImageManagerImageList* pImageList); //<combine CXTPImageManagerIcon::CXTPImageManagerIcon@UINT@int@int@CXTPImageManagerIconSet*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerIcon object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManagerIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the image identifier.
	// Returns:
	//     Identifier of the image
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the image has alpha channel.
	// Returns:
	//     TRUE if the image has alpha channel; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL IsAlpha() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Checks if icons was scaled from another CXTPImageManagerIcon object.
	//-------------------------------------------------------------------------
	BOOL IsScaled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon handle.
	// Parameters:
	//     imageState - Image state for icon to retrieve
	// Returns:
	//     Icon handle.
	// See Also: XTPImageState
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetIcon();
	CXTPImageManagerIconHandle& GetIcon(XTPImageState imageState); // <combine CXTPImageManagerIcon::GetIcon>

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the faded icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetFadedIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the shadow for the icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetShadowIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the shadow for the icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetCheckedIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the pressed icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetPressedIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves hot icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetHotIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the disabled icon.
	// Parameters:
	//     bCreateIfNotExists - Create if image not exists.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetDisabledIcon(BOOL bCreateIfNotExists = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if disabled icon exists
	// Returns:
	//     TRUE if disabled icon exists
	//-----------------------------------------------------------------------
	BOOL HasDisabledIcon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a disabled icon.
	// Parameters:
	//     clrDisabledLight - The lightest color of icon to generate
	//     clrDisabledDark -  The darkest color of icon to generate
	//-----------------------------------------------------------------------
	void CreateDisabledIcon(COLORREF clrDisabledLight = (COLORREF)-1, COLORREF clrDisabledDark = (COLORREF)-1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//     nIDResourceIcon - Icon resource identifier.
	//     nWidth - Width of icon.
	//     nHeight - Height of icon.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL SetIcon(CXTPImageManagerIconHandle hIcon);
	BOOL SetIcon(CXTPImageManagerIconHandle hIcon, XTPImageState imageState); // <combine CXTPImageManagerIcon::SetIcon@CXTPImageManagerIconHandle>
	BOOL SetIcon(UINT nIDResourceIcon, int nWidth, int nHeight); // <combine CXTPImageManagerIcon::SetIcon@CXTPImageManagerIconHandle>

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the disabled icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetDisabledIcon(CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the hot icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetHotIcon(CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the checked icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetCheckedIcon(CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetNormalIcon(CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the pressed icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetPressedIcon(CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the size of the icon.
	// Parameters:
	//     hIcon - Icon handle.
	// Returns:
	//     Size of the icon.
	//-----------------------------------------------------------------------
	CSize GetExtent() const;
	static CSize AFX_CDECL GetExtent(HICON hIcon); // <COMBINE CXTPImageManagerIcon::GetExtent@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method tests if file is alpha bitmap.
	// Parameters:
	//     pszFileName - File path.
	// Returns:
	//     TRUE if file is alpha bitmap
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaBitmapFile(LPCTSTR pszFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method tests if file is png image.
	// Parameters:
	//     pszFileName - File path.
	// Returns:
	//     TRUE if file is png image
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsPngBitmapFile(LPCTSTR pszFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads alpha bitmap from icon file.
	// Parameters:
	//     pszFileName - File path.
	//     nWidth - Icon width to load.
	// Returns:
	//     Alpha bitmap if icon with specified width was found; otherwise returns NULL.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadAlphaIcon(LPCTSTR pszFileName, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads Alpha bitmap from resource file
	// Parameters:
	//     nIDResource - Resource identifier
	//     hModule - Handle to the instance of the module whose executable file contains the bitmap to be loaded.
	//     lpszResource - Long pointer to a null-terminated string that contains the name of the bitmap resource to be loaded
	// Remarks:
	//     Don't use LoadImage to load alpha bitmap, it loose alpha channel in some Windows.
	// Returns:
	//     Alpha bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadAlphaBitmap(UINT nIDResource);
	static HBITMAP AFX_CDECL LoadAlphaBitmap(HMODULE hModule, LPCTSTR lpszResource); // <combine CXTPImageManagerIcon::LoadAlphaBitmap@UINT>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads bitmap from file on disk
	// Parameters:
	//     lpszFileName - File name of bitmap or png file
	//     lbAlphaBitmap - Determines if bitmap contained alpha layer
	// Remarks:
	//     This method loads png or bitmap from file
	// Returns:
	//     Bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadBitmapFromFile(LPCTSTR lpszFileName, BOOL* lbAlphaBitmap = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads Alpha bitmap from resource file
	// Parameters:
	//     hModule - Handle to the instance of the module whose executable file contains the bitmap to be loaded.
	//     lpszResource - Long pointer to a null-terminated string that contains the name of the bitmap resource to be loaded
	//     lbAlphaBitmap - Determines if bitmap contained alpha layer
	// Remarks:
	//     This method loads png or bitmap resource from file
	// Returns:
	//     Alpha bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadBitmapFromResource(LPCTSTR lpszResource, BOOL* lbAlphaBitmap);
	static HBITMAP AFX_CDECL LoadBitmapFromResource(HMODULE hModule, LPCTSTR lpszResource, BOOL* lbAlphaBitmap); // <combine CXTPImageManagerIcon::LoadBitmapFromResource@LPCTSTR@BOOL*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if bitmap with specified resource has alpha channel.
	// Parameters:
	//     hModule - Module of the bitmap to check.
	//     lpBitmapName - Resource name of the bitmap to check.
	// Returns:
	//     TRUE if bitmap has alpha channel; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaBitmapResource(HMODULE hModule, LPCTSTR lpBitmapName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if bitmap with specified resource is png resource.
	// Parameters:
	//     hModule - Module of the bitmap to check.
	//     lpBitmapName - Resource name of the bitmap to check.
	// Returns:
	//     TRUE if bitmap is png resource; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsPngBitmapResource(HMODULE hModule, LPCTSTR lpBitmapName);

	//-----------------------------------------------------------------------
	// Summary:
	//     The DrawAlphaBitmap function displays bitmaps that have transparent or semitransparent pixels.
	// Parameters:
	//     pDC     - Pointer to destination device context
	//     ptDest      - upper-left corner of the  destination rectangle
	//     ptSrc      - upper-left corner of the source rectangle
	//     hBitmap - Alpha bitmap handler
	//     szDest  - Destination size
	//     szSrc   - Source size
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawAlphaBitmap(CDC* pDC, HBITMAP hBitmap, CPoint ptDest, CSize szDest, CPoint ptSrc = 0, CSize szSrc = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates valid alpha bits to use in AlphaBlend.
	// Parameters:
	//     hBitmap - Alpha bitmap handler
	// Returns:
	//     Valid alpha bits
	// See Also: BLENDFUNCTION, AlphaBlend
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL PreMultiplyAlphaBitmap(HBITMAP hBitmap, BOOL* pbAlpha = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create mirror copy of bitmap
	// Parameters:
	//     hBitmap - Bitmap handle to make copy
	// Returns: new mirror bitmap
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL InvertAlphaBitmap(HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     This helper method retrieves information about alpha bitmap and its alpha bits.
	// Parameters:
	//     dcSrc   - Reference to CDC
	//     hBitmap - Alpha bitmap
	//     pbmi    - Bitmap information
	//     pBits   - Bitmap bits
	//     nSize   - Size of bits array
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL GetBitmapBits(CDC& dcSrc, HBITMAP hBitmap, PBITMAPINFO& pbmi, LPVOID& pBits, UINT& nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies alpha bitmap.
	// Parameters:
	//     hBitmap - Bitmap handler to be copied.
	// Returns:
	//     new alpha bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL CopyAlphaBitmap(HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon height.
	// Returns:
	//     Height of the icon.
	//-----------------------------------------------------------------------
	int GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon width.
	// Returns:
	//     Width of the icon.
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	//-----------------------------------------------------------------------
	void Serialize(CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     hIcon - Icon to be serialized
	//     ar - A CArchive object to serialize to or from.
	//-----------------------------------------------------------------------
	void SerializeIcon(CXTPImageManagerIconHandle& hIcon, CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image in specified location.
	// Parameters:
	//     pDC    - Destination device context.
	//     pt     - Specifies the location of the image
	//     hIcon  - A handle to an icon.
	//     szIcon - Specifies the size of the image.
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC, CPoint pt);
	void Draw(CDC* pDC, CPoint pt, CSize szIcon); // <combine CXTPImageManagerIcon::Draw@CDC*@CPoint>
	void Draw(CDC* pDC, CPoint pt, CXTPImageManagerIconHandle& hIcon, CSize szIcon = 0); // <combine CXTPImageManagerIcon::Draw@CDC*@CPoint>

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image using the brush specified by the clrBrush parameter.
	// Parameters:
	//     pDC      - Destination device context.
	//     pt       - Specifies the location of the image.
	//     hIcon    - A handle to an icon.
	//     clrBrush - color of the brush used to draw the image.
	//-----------------------------------------------------------------------
	void DrawMono(CDC* pDC, CPoint pt, CXTPImageManagerIconHandle& hIcon, CSize szIcon, COLORREF clrBrush);

	//-------------------------------------------------------------------------
	// Summary:
	//     Redraw Fade and Shadow icons.
	//-------------------------------------------------------------------------
	void Refresh();


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Use this method to scale icon.
	// Parameters:
	//     hIcon - Icon to be scaled.
	//     szExtent - Source icon size.
	//     nWidth - With of result icon.
	//-----------------------------------------------------------------------
	static HICON AFX_CDECL ScaleToFit(HICON hIcon, CSize szExtent, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw non-alpha icon in composited Vista Glass rectangle
	// Parameters:
	//     pDC - Pointer to device context
	//     pt - Pointer to draw icon
	//     szIcon - Size of the icon
	//     hIcon - Icon handle to draw
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawIconComposited(CDC* pDC, CPoint pt, CSize szIcon, HICON hIcon);

private:
	void InitBimaps(CImageList& imageList, int nIndex);
	void CreateFadedIcon();
	void CreateShadowIcon();

	void Clear(BOOL bIcon = FALSE);
	COLORREF LightenColor(COLORREF clr, double factor);

	BOOL GetDIBBitmap(HBITMAP hBitmap, PBYTE& pBits, UINT& nBitsSize, PBITMAPINFO& pBitmapInfo, UINT& nBitmapInfoSize);
	HBITMAP ReadDIBBitmap(CArchive& ar);
	void WriteDIBBitmap(CArchive& ar, HBITMAP hBitmap);

	CXTPImageManager* GetImageManager() const;

private:
	CXTPImageManagerIcon(const CXTPImageManagerIcon&): m_nID(0) {};
	const CXTPImageManagerIcon& operator=(const CXTPImageManagerIcon&) {return *this;};

public:
	BOOL m_bDrawComposited; // Draw on Vista composited rects.

private:
	const UINT m_nID;
	UINT m_nWidth;
	UINT m_nHeight;
	CXTPImageManagerIconHandle m_hIcon;
	CXTPImageManagerIconHandle m_hFaded;
	CXTPImageManagerIconHandle m_hShadow;
	CXTPImageManagerIconHandle m_hHot;
	CXTPImageManagerIconHandle m_hChecked;
	CXTPImageManagerIconHandle m_hPressed;
	CXTPImageManagerIconHandle m_hDisabled;
	CXTPImageManagerIconHandle m_hDisabledAuto;
	BOOL m_bScaled;
	CXTPImageManagerIconSet* m_pIconSet;
	CXTPImageManagerImageList* m_pImageList;

private:
	friend class CXTPImageManager;
	friend class CXTPImageManagerIconSet;
	friend class CXTPImageManagerIconHandle;

};


//===========================================================================
// Summary:
//     CXTPImageManagerIconSet represents the collection of icons for specified identifier.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerIconSet : public CXTPCmdTarget
{
public:
	typedef CMap<UINT, UINT, CXTPImageManagerIcon*, CXTPImageManagerIcon*> CIconSetMap; // Icons collection.

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManagerIconSet object.
	// Parameters:
	//     nID - Icon set identifier.
	//     pManager - Parent image manager CXTPImageManagerIconSet belongs.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconSet(UINT nID, CXTPImageManager* pManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerIconSet object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManagerIconSet();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon with the specified width
	// Parameters:
	//     nWidth - Width of the icon to retrieve.
	//     bScaled - TRUE to scale icon.
	// Returns:
	//     A pointer to a CXTPImageManagerIcon object
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon(UINT nWidth, BOOL bScaled = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the collection of icons
	// Returns:
	//     A pointer to a CIconSetMap object
	//-----------------------------------------------------------------------
	CIconSetMap* GetIcons();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the identifier of the collection set.
	// Returns:
	//     Identifier of icons.
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Removes all icons
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-------------------------------------------------------------------------
	// Summary:
	//     Redraws all icons.
	//-------------------------------------------------------------------------
	void RefreshAll();

private:
	CXTPImageManagerIcon* CreateIcon(UINT nWidth);
	void SetIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth);
	void SetDisabledIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth);
	void SetHotIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth);
	void SetCheckedIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth);
	void SetPressedIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth);

private:
	CXTPImageManagerIconSet(const CXTPImageManagerIconSet&): m_nID(0) {};
	const CXTPImageManagerIconSet& operator=(const CXTPImageManagerIconSet&) {return *this;};

private:
	CIconSetMap m_mapIcons;
	const UINT m_nID;
	CXTPImageManager* m_pManager;

private:
	friend class CXTPImageManager;
	friend class CXTPImageManagerIcon;
};

//{{AFX_CODEJOCK_PRIVATE

class _XTP_EXT_CLASS CXTPImageManagerImageList
{
public:
	CXTPImageManagerImageList();
	~CXTPImageManagerImageList();

public:
	BOOL Lookup(UINT nCommand);
	CXTPImageManagerIcon* GetIcon(UINT nCommand);
	void Draw(CDC* pDC, CPoint pt, CXTPImageManagerIcon* pIcon, CSize szIcon);
	void RemoveAll();

	CSize GetIconSize() const;

private:
	HIMAGELIST m_hImageList;
	BOOL m_bDestroyImageList;
	UINT m_nBaseCommand;
	CXTPImageManager* m_pManager;
	CMap<UINT, UINT, CXTPImageManagerIcon*, CXTPImageManagerIcon*> m_mapIcons;

	friend class CXTPImageManager;
};

//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPImageManager is a stand alone class. It is used to manipulate icons.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManager : public CXTPCmdTarget
{
private:
	typedef BOOL (__stdcall* PFNALPHABLEND) (HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION); // AlphaBlend function declaration
	typedef BOOL (WINAPI* PFNTRANSPARENTBLT)(HDC, int, int, int, int, HDC, int, int, int, int, UINT);           // TransparentBlt  function declaration

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManager object.
	//-----------------------------------------------------------------------
	CXTPImageManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManager object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManager();
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds the icons.
	// Parameters:
	//     nIDResourceBitmap  - Bitmap resource identifier
	//     nIDResourceToolBar - Toolbar resource identifier
	//     bmpIcons           - Bitmap containing icons to be added.
	//     imlIcons           - Image list containing the icons to be added.
	//     pCommands          - Pointer to an array of IDs.
	//     nCount             - Number of elements in the array pointed to by lpIDArray.
	//     szIcon             - Size of the icons.
	//     imageState         - Images state.
	//     bAlpha             - TRUE if bitmap has alpha channel.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	// Example:
	// <code>
	//
	// static UINT buttons[] =
	// {
	//     ID_BACK,
	//     ID_FORWARD,
	//     ID_STOP,
	//     ID_REFRESH,
	//     ID_HOME
	// }
	//
	// XTPImageManager()->SetMaskColor(RGB(0, 0xFF, 0));
	//
	// XTPImageManager()->SetIcons(IDR_TOOLBAR_NORMAL_SMALL, buttons, _countof(buttons), CSize(16, 16), xtpImageNormal);
	// XTPImageManager()->SetIcons(IDR_TOOLBAR_HOT_SMALL, buttons, _countof(buttons), CSize(16, 16), xtpImageHot);
	//
	// XTPImageManager()->SetIcons(IDR_TOOLBAR_NORMAL_LARGE, buttons, _countof(buttons), CSize(24, 24), xtpImageNormal);
	// XTPImageManager()->SetIcons(IDR_TOOLBAR_HOT_LARGE, buttons, _countof(buttons), CSize(24, 24), xtpImageHot);
	// </code>
	// See Also: XTPImageState, SetIcon
	//-----------------------------------------------------------------------
	BOOL SetIcons(UINT nIDResourceToolBar, XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(UINT nIDResourceToolBar, UINT nIDResourceBitmap, XTPImageState imageState = xtpImageNormal); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>
	BOOL SetIcons(CBitmap& bmpIcons, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState = xtpImageNormal, BOOL bAlpha = FALSE); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>
	BOOL SetIcons(UINT nIDResourceToolBar, CBitmap& bmpIcons, XTPImageState imageState = xtpImageNormal); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>
	BOOL SetIcons(UINT nIDResourceBitmap, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState = xtpImageNormal); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>
	BOOL SetIcons(LPCTSTR lpszResourceBitmap, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState = xtpImageNormal); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>
	BOOL SetIcons(CImageList& imlIcons, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState = xtpImageNormal); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>
	BOOL SetIcons(UINT nIDResourceToolBar, CImageList& imlIcons, XTPImageState imageState = xtpImageNormal); // <COMBINE CXTPImageManager::SetIcons@UINT@XTPImageState>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads icon from ico file
	// Parameters:
	//     lpszFileName - Path to ico file
	//     nIDCommand   - Icon command
	//     szIcon       - Size of the icons.
	//     imageState   - Images state.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL SetIconFromIcoFile(LPCTSTR lpszFileName, UINT nIDCommand, CSize szIcon, XTPImageState imageState);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this copy images from a HIMAGELIST control to the image manager
	// Parameters:
	//     hImageList - Pointer to an imagelist that holds icons
	//     nBaseCommand - ID to start numbering icons from the imagelist
	//     bDestoryImageList - Destroy imagelist control after the icons are copied to the imagemanager
	//-----------------------------------------------------------------------
	void SetImageList(HIMAGELIST hImageList, int nBaseCommand, BOOL bDestoryImageList = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds the icons from another image manager.
	// Parameters:
	//     pImageManager - A pointer to another image manager. The icons in pImageManager will be added to this image manager.
	//-----------------------------------------------------------------------
	void AddIcons(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds the icon.
	// Parameters:
	//     hIcon           - Icon handle to add
	//     nIDResourceIcon - Icon resource identifier
	//     nIDCommand      - Icon identifier.
	//     szIcon          - Size of the icons.
	//     imageState      - Images state.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	// See Also: XTPImageState, SetIcons
	//-----------------------------------------------------------------------
	BOOL SetIcon(CXTPImageManagerIconHandle hIcon, UINT nIDCommand, CSize szIcon = 0, XTPImageState imageState = xtpImageNormal);
	BOOL SetIcon(UINT nIDResourceIcon, UINT nIDCommand, CSize szIcon = 0, XTPImageState imageState = xtpImageNormal); // <combine CXTPImageManager::SetIcon@CXTPImageManagerIconHandle@UINT@CSize@XTPImageState>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the color mask of the bitmaps that will be added.
	// Parameters:
	//     clrMask - Mask color to be set.
	// Returns:
	//     Previous color mask.
	//-----------------------------------------------------------------------
	COLORREF SetMaskColor(COLORREF clrMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves color mask.
	// Returns:
	//     Current color mask.
	//-----------------------------------------------------------------------
	COLORREF GetMaskColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Mirrors images around the y-axis, useful in RTL (Right-To-Left) Languages.
	// Parameters:
	//     bDrawReverted - If TRUE, images are "flipped" or Mirrored.
	//                     This should be used when displaying images in an
	//                     application that uses RTL (Right-To-Left) Languages.
	//-----------------------------------------------------------------------
	void DrawReverted(BOOL bDrawReverted);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all icons.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes the specified icon.
	// Parameters:
	//     nIDCommand - Id of icon to be removed
	//-----------------------------------------------------------------------
	void RemoveIcon(UINT nIDCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//     Redraws all icons.
	//-----------------------------------------------------------------------
	void RefreshAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method used by CommandBars to add custom icons
	// Parameters:
	//     hIcon - Icon Handler to be added
	// Returns:
	//     Identifier of new added icon.
	//-----------------------------------------------------------------------
	UINT AddCustomIcon(CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icons with the specified Id and width.
	// Parameters:
	//     nCommand - ID of the icon to be retrieved
	//     nWidth - Width of the icon to be retrieved
	// Returns:
	//     A pointer to a CXTPImageManagerIcon
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetImage(UINT nCommand, int nWidth = 16) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if non scaled image exists in image manager
	// Parameters:
	//     nCommand - Image identifier to check
	//     nWidth - Image width to check
	// Returns:
	//     TRUE if image non scaled image exists.
	// See Also: GetImage
	//-----------------------------------------------------------------------
	BOOL IsPrimaryImageExists(UINT nCommand, int nWidth) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icons set with the specified Id.
	// Parameters:
	//     nCommand - ID of the icon set to be retrieved
	// Returns:
	//     A pointer to a CXTPImageManagerIconSet object
	//-----------------------------------------------------------------------
	CXTPImageManagerIconSet* GetIconSet(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icons set with the specified Id from an image list.
	// Parameters:
	//     nCommand - ID of the icon set to be retrieved
	// Returns:
	//     A pointer to a CXTPImageManagerImageList object
	//-----------------------------------------------------------------------
	CXTPImageManagerImageList* GetImageList(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a collection of images.
	// Returns:
	//     A CMap object
	//-----------------------------------------------------------------------
	CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*>* GetImages();

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if icons set exists with the specified ID.
	// Parameters:
	//     nCommand - Identifier needed to check.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL Lookup(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the icons are drawn mirrored\reverted.
	// Returns:
	//     TRUE if icons are drawn reverted; FALSE if drawn normal.
	//-----------------------------------------------------------------------
	BOOL IsDrawReverted() const;
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes icons from or to an archive.
	// Parameters:
	//     nCommand - Specified command to serialize.
	//     ar - A CArchive object to serialize to or from.
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//-----------------------------------------------------------------------
	void Serialize(CArchive& ar);
	void Serialize(UINT nCommand, CArchive& ar); //<COMBINE CXTPImageManager::Serialize@CArchive&>

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes icons from or to an archive.
	// Parameters:
	//     nCommand - Specified command to serialize.
	//     ar - A CArchive object to serialize to or from.
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//-----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);
	void DoPropExchange(UINT nCommand, CXTPPropExchange* pPX); //<COMBINE CXTPImageManager::DoPropExchange@CXTPPropExchange*>

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if bitmap with specified resource has alpha channel.
	// Parameters:
	//     nIDResourceBitmap - Resource identifier of the bitmap to check.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaBitmapResource(UINT nIDResourceBitmap);
	static BOOL AFX_CDECL IsPngBitmapResource(UINT nIDResourceBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the OS supports alpha icons.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	// See Also: IsAlphaIconsImageListSupported
	//-----------------------------------------------------------------------
	BOOL IsAlphaIconsSupported() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the OS supports alpha image list.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	// See Also: IsAlphaIconsSupported
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaIconsImageListSupported();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves color of top-left pixel.
	// Parameters:
	//     bmp       - CBitmap reference.
	//     nIDBitmap - Bitmap identifier.
	//     pt        - location of pixel to retrieve.
	// Returns:
	//     COLORREF that will be used as transparent color.
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL GetBitmapMaskColor(CBitmap& bmp, CPoint pt = 0);
	static COLORREF AFX_CDECL GetBitmapMaskColor(UINT nIDBitmap, CPoint pt = 0); // <COMBINE CXTPImageManager::GetBitmapMaskColor@CBitmap&@CPoint>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method creates 24bit bitmap from 32bit alpha bitmaps to draw it in old OSes
	// Parameters:
	//     bmpAlpha - Resource bitmap identifier.
	//     clrMask  - Mask color to be used
	//     nIDBitmap - Resource bitmap identifier.
	// Returns:
	//     Reduced bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL ResampleAlphaLayer(HBITMAP bmpAlpha, COLORREF clrMask);
	static HBITMAP AFX_CDECL ResampleAlphaLayer(UINT nIDBitmap, COLORREF clrMask); // <COMBINE CXTPImageManager::ResampleAlphaLayer@HBITMAP@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     The TransparentBlt function performs a bit-block transfer of the color data corresponding to
	//     a rectangle of pixels from the specified source device context into a destination device context.
	// Parameters:
	//     hdcDest - handle to destination DC
	//     rcDest - destination rectangle
	//     hdcSrc - handle to source DC
	//     rcSrc - source rectangle
	//     crTransparent - color to make transparent
	// Returns:
	//     If the function succeeds, the return value is TRUE.
	// See Also: AlphaBlend
	//-----------------------------------------------------------------------
	BOOL TransparentBlt(HDC hdcDest, const CRect& rcDest,HDC hdcSrc, const CRect& rcSrc, UINT crTransparent) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The AlphaBlend function displays bitmaps that have transparent or semitransparent pixels.
	// Parameters:
	//     hdcDest - handle to destination DC
	//     rcDest - destination rectangle
	//     hdcSrc - handle to source DC
	//     rcSrc - source rectangle
	// Returns:
	//     If the function succeeds, the return value is TRUE.
	// See Also: TransparentBlt
	//-----------------------------------------------------------------------
	BOOL AlphaBlend(HDC hdcDest, const CRect& rcDest,HDC hdcSrc, const CRect& rcSrc) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This function make a rectangle on device context as disabled.
	// Parameters:
	//     hDC              - A handle to DC
	//     rcRect           - A rectangle to be disabled
	//     clrDisabledLight - The lightest color of icon to generate
	//     clrDisabledDark  - The darkest color of icon to generate
	// Returns:
	//     If the function succeeds, the return value is TRUE.
	// See Also: BlackWhiteBitmap
	//-----------------------------------------------------------------------
	BOOL DisableBitmap(HDC hDC, const CRect& rcRect, COLORREF clrDisabledLight = (COLORREF)-1, COLORREF clrDisabledDark = (COLORREF)-1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function make a rectangle on device context as gray scale.
	// Parameters:
	//     hDC                    - A handle to DC
	//     rcRect                 - A rectangle to be disabled
	//     nBlackAndWhiteContrast - Black-white contrast as integer from 0 to 255
	// Returns:
	//     If the function succeeds, the return value is TRUE.
	// See Also: DisableBitmap
	//-----------------------------------------------------------------------
	BOOL BlackWhiteBitmap(HDC hDC, const CRect& rcRect, int nBlackAndWhiteContrast = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function used to get a bitmap pixel color (including alpha layer).
	// Parameters:
	//     hBmp      - [in] A handle to bitmap
	//     pt        - [in] Coordinates of bitmap pixel.
	//     rclrPixel - [out] A reference to receive pixel color.
	// Returns:
	//     If the function succeeds, the return value is TRUE.
	//-----------------------------------------------------------------------
	BOOL GetBitmapPixel(HBITMAP hBmp, const CPoint& pt, COLORREF& rclrPixel);

	//-----------------------------------------------------------------------
	// Summary:
	//     This helper method creates 32bit bitmap
	// Parameters:
	//     hDC - Pointer to valid context (can be NULL)
	//     iWidth - Width of bitmap
	//     iHeight - Height of bitmap
	//     lpBits - Pointer to bits to return
	// Returns:
	//     Bitmap handle was created.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL Create32BPPDIBSection(HDC hDC, int iWidth, int iHeight, LPBYTE* lpBits = NULL);
private:
	BOOL LoadToolbar(UINT nIDResourceToolBar, UINT*& pItems, int& nCount, CSize& szIcon) const;

	UINT PasteCustomImage(COleDataObject& data);
	void CopyImage(UINT nCommand);
	BOOL SplitBitmap(HBITMAP hbmSource, int nCount, HBITMAP* pDest) const;
	BOOL IsWindow2000() const;
	BOOL BitmapsCompatible(LPBITMAP lpbm1, LPBITMAP lpbm2) const;
	BOOL McTransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
		int nWidthDest, int nHeightDest, HDC hdcSrc,
		int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
		int nHeightSrc, UINT crTransparent) const;
	BOOL BlendImages(HBITMAP hbmSrc1, BOOL bRTL1, HBITMAP hbmSrc2, BOOL bRTL2, HBITMAP hbmDst) const;
	BOOL DoAlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest,
		int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc,
		int nYOriginSrc, int nWidthSrc, int nHeightSrc) const;
	BOOL DoDisableBitmap(HBITMAP hBmp, COLORREF clrDisabledLight = (COLORREF)-1, COLORREF clrDisabledDark = (COLORREF)-1,
						 int nBlackAndWhiteContrast = -1);

public:
	static double m_dDisabledBrightnessFactor;  // Brightness factor of disabled icons.
	static double m_dDisabledAlphaFactor;       // Alpha factor of disabled icons.
	static BOOL   m_bAutoResample;              // TRUE to automatically re-sample alpha bitmaps if msimg32 not found.
	static CLIPFORMAT m_nImageClipFormat;       // Clipboard format of icons.
	static CLIPFORMAT m_nAlphaClipFormat;       // Clipboard format of icons.

private:
	COLORREF m_clrMask;
	CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*> m_mapImages;
	CArray<CXTPImageManagerImageList*, CXTPImageManagerImageList*> m_arrImageList;


	UINT m_nCustomID;
	HMODULE m_hMsImgDll;
	PFNALPHABLEND m_pfnAlphaBlend;
	PFNTRANSPARENTBLT m_pfnTransparentBlt;
	BOOL m_bDrawReverted;

private:
	friend _XTP_EXT_CLASS CXTPImageManager* AFX_CDECL XTPImageManager();
	friend class CXTPCustomizeSheet;
	friend class CXTPImageManagerIcon;

};

//===========================================================================
// Summary:
//    CXTPTempColorMask class can be used to temporarily set the color mask
//    used by XTPImageManager when using images with different color masks.
//    When the destructor is called the default mask is reset for
//    XTPImageManager.
// Example:
//    The following example demonstrates using CXTPTempColorMask.
// <code>
//    // temporarily change the mask color.
//    CXTPTempColorMask mask(RGB(0,255,0));
//    XTPImageManager()->SetIcons(IDB_FAVORITES,
//        icons, _countof(icons) CSize(16,16));
//
//    // set another mask color, CXTPTempColorMask will handle resetting
//    // the default color mask when it is destroyed.
//    XTPImageManager()->SetMaskColor(RGB(255,0,255));
//    XTPImageManager()->SetIcons(IDR_TOOLBAR_MASKED,
//        buttons, _countof(buttons), CSize(16,16), xtpImageNormal);
// </code>
// See Also:
//    CXTPImageManager::GetMaskColor, CXTPImageManager::SetMaskColor,
//    CXTPPaintManagerColor::GetStandardColor
//========================================================================
class _XTP_EXT_CLASS CXTPTempColorMask
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTempColorMask object and initializes the
	//     application defined mask color and saves the default mask color
	//     used by XTPImageManager.
	// Parameters:
	//     clr - An RGB value that represents the new color mask value used
	//           by XTPImageManager.
	//-----------------------------------------------------------------------
	CXTPTempColorMask(COLORREF crNewMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destructor for CXTPTempColorMask. Resets the default mask color
	//     used by XTPImageManager back to its original color value.
	//-----------------------------------------------------------------------
	virtual ~CXTPTempColorMask();

private:
	COLORREF m_crMask;
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPImageManagerIconHandle::operator HICON() const {
	return m_hIcon;
}

AFX_INLINE UINT CXTPImageManagerIconSet::GetID() const {
	return m_nID;
}

_XTP_EXT_CLASS CXTPImageManager* AFX_CDECL XTPImageManager();


AFX_INLINE CXTPImageManagerIconSet::CIconSetMap* CXTPImageManagerIconSet::GetIcons() {
	return &m_mapIcons;
}
AFX_INLINE CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*>* CXTPImageManager::GetImages() {
	return  &m_mapImages;
}

AFX_INLINE void CXTPImageManager::DrawReverted(BOOL bDrawReverted) {
	m_bDrawReverted = bDrawReverted;
}
AFX_INLINE BOOL CXTPImageManager::IsDrawReverted() const {
	return m_bDrawReverted;
}

AFX_INLINE CXTPTempColorMask::CXTPTempColorMask(COLORREF crNewMask) {
	m_crMask = XTPImageManager()->SetMaskColor(crNewMask);
}
AFX_INLINE CXTPTempColorMask::~CXTPTempColorMask() {
	XTPImageManager()->SetMaskColor(m_crMask);
}

#endif //#if !defined(__XTPIMAGEMANAGER_H__)
