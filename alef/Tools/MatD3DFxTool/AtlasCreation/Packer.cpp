//-----------------------------------------------------------------------------
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL 
// NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
// CONSEQUENTIAL DAMAGES WHATSOEVER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
// LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
// INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR 
// INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGES.
// 
// File: Packer.cpp
// Desc: Packer class implementation.
//-----------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>

#include "Packer.h"
#include "TextureObject.h"

//-----------------------------------------------------------------------------
// Name: Region()
// Desc: Constructor
//-----------------------------------------------------------------------------
Region::Region()
{
    ;
}

//-----------------------------------------------------------------------------
// Name: Region()
// Desc: Destructor
//-----------------------------------------------------------------------------
Region::~Region()
{
    ;
}

//-----------------------------------------------------------------------------
// Name: GetWidth()
// Desc: returns width of region
//-----------------------------------------------------------------------------
long Region::GetWidth() const
{
    assert(mRight >= mLeft);
    return (mRight - mLeft);
}

//-----------------------------------------------------------------------------
// Name: GetHeight()
// Desc: returns height of region
//-----------------------------------------------------------------------------
long Region::GetHeight() const
{
    assert(mBottom >= mTop);
    return (mBottom - mTop);
}

//-----------------------------------------------------------------------------
// Name: Intersect()
// Desc: returns true if the passed in region intersects this region
//-----------------------------------------------------------------------------
bool Region::Intersect(Region const &region) const
{
    // Note Region is defined as [left, right) and [top, bottom)!
    // general tests: test whether region's edges are inside this region
    bool const leftIsInside   = (region.mLeft   >= mLeft) && (region.mLeft   <  mRight);
    bool const rightIsInside  = (region.mRight  >  mLeft) && (region.mRight  <= mRight);
    bool const topIsInside    = (region.mTop    >= mTop ) && (region.mTop    <  mBottom);
    bool const bottomIsInside = (region.mBottom >  mTop ) && (region.mBottom <= mBottom);

    // if and only if (at least) one of the horizontal edges and 
    //                (at least) one of the vertical edges inside than the two inersect
    if ((leftIsInside || rightIsInside) && (topIsInside || bottomIsInside))
        return true;
    return false;
}

//-----------------------------------------------------------------------------
// Name: Packer()
// Desc: Constructor
//-----------------------------------------------------------------------------
Packer::Packer()
    : mTotalFreeTexels(0)
    , mMaxNumberMipLevels(0)

{
    ;
}

//-----------------------------------------------------------------------------
// Name: Packer()
// Desc: Constructor
//-----------------------------------------------------------------------------
Packer::~Packer()
{
    ;
}

//-----------------------------------------------------------------------------
// Name: SizeOfTexel()
// Desc: Returns the size (in bits) of the given texel format
//-----------------------------------------------------------------------------
int Packer::SizeOfTexel( D3DFORMAT format ) const
{
	switch( format )
	{
		case D3DFMT_R8G8B8:			return 3*8;
		case D3DFMT_A8R8G8B8:		return 4*8;
		case D3DFMT_X8R8G8B8:		return 4*8;
		case D3DFMT_R5G6B5:			return 2*8;
		case D3DFMT_X1R5G5B5:		return 2*8;
		case D3DFMT_A1R5G5B5:		return 2*8;
		case D3DFMT_A4R4G4B4:		return 2*8;
		case D3DFMT_R3G3B2:			return 8;
		case D3DFMT_A8:				return 8;
		case D3DFMT_A8R3G3B2:		return 2*8;
		case D3DFMT_X4R4G4B4:		return 2*8;
		case D3DFMT_A2B10G10R10:	return 4*8;
		case D3DFMT_A8B8G8R8:		return 4*8;
		case D3DFMT_X8B8G8R8:		return 4*8;
		case D3DFMT_G16R16:			return 4*8;
		case D3DFMT_A2R10G10B10:	return 4*8;
		case D3DFMT_A16B16G16R16:	return 8*8;
		case D3DFMT_A8P8:			return 8;
		case D3DFMT_P8:				return 8;
		case D3DFMT_L8:				return 8;
		case D3DFMT_L16:			return 2*8;
		case D3DFMT_A8L8:			return 2*8;
		case D3DFMT_A4L4:			return 8;
		case D3DFMT_V8U8:			return 2*8;
		case D3DFMT_Q8W8V8U8:		return 4*8;
		case D3DFMT_V16U16:			return 4*8;
		case D3DFMT_Q16W16V16U16:	return 8*8;
		case D3DFMT_CxV8U8:			return 2*8;
		case D3DFMT_L6V5U5:			return 2*8;
		case D3DFMT_X8L8V8U8:		return 4*8;
		case D3DFMT_A2W10V10U10:	return 4*8;
		case D3DFMT_G8R8_G8B8:		return 2*8;
		case D3DFMT_R8G8_B8G8:		return 2*8;
		case D3DFMT_DXT1:			return 4;
		case D3DFMT_DXT2:			return 8;
		case D3DFMT_DXT3:			return 8;
		case D3DFMT_DXT4:			return 8;
		case D3DFMT_DXT5:			return 8;
		case D3DFMT_UYVY:			return 2*8;
		case D3DFMT_YUY2:			return 2*8;
		case D3DFMT_R16F:			return 2*8;
		case D3DFMT_G16R16F:		return 4*8;
		case D3DFMT_A16B16G16R16F:	return 8*8;
		case D3DFMT_R32F:			return 4*8;
		case D3DFMT_G32R32F:		return 8*8;
		case D3DFMT_A32B32G32R32F:	return 16*8;
		default:
			return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: IsDXTnFormat()
// Desc: Returns true if the given format is a DXT format, false otherwise
//-----------------------------------------------------------------------------
bool Packer::IsDXTnFormat( D3DFORMAT format ) const 
{
	switch( format )
	{
		case D3DFMT_DXT1:		
		case D3DFMT_DXT2:		
		case D3DFMT_DXT3:		
		case D3DFMT_DXT4:		
		case D3DFMT_DXT5:		
			return true;
		default:
			return false;
	}
}

//-----------------------------------------------------------------------------
// Name: GetMaxMipLevels()
// Desc: Returns the largest number of mip-maps of all textures passed into 
//       Insert() function.
//-----------------------------------------------------------------------------
int Packer::GetMaxMipLevels() const 
{
    return mMaxNumberMipLevels;
}


//-----------------------------------------------------------------------------
// Name: Packer2D()
// Desc: Constructor
//-----------------------------------------------------------------------------
Packer2D::Packer2D(Atlas2D * pAtlas)
    : mpAtlas(pAtlas)
{
    assert(pAtlas != NULL);

    mTotalFreeTexels = pAtlas->GetNumTexels();
}

//-----------------------------------------------------------------------------
// Name: Packer2D()
// Desc: Destructor
//-----------------------------------------------------------------------------
Packer2D::~Packer2D()
{
    std::vector<Region *>::iterator  iterRegion;
    
    for (iterRegion = mUsedRegions.begin(); iterRegion != mUsedRegions.end(); ++iterRegion)
    {
        delete *iterRegion;
        *iterRegion = NULL;
    }
    mpAtlas = NULL;
}

//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Insert passed-in texture into atlas.  If no free spot is found, 
//       return false.  If a free spot is found, copy bits from texture 
//       to atlas, update texture to point to proper sub-region in atlas
//       add the newly filled region to the used region vector, then
//       return true.
//-----------------------------------------------------------------------------
bool Packer2D::Insert(Texture2D *pTexture)
{
    // check for trivial non-fit
    if (pTexture->GetNumTexels() > mTotalFreeTexels)
        return false;

    // find a free spot for this texture
    Region *pTest = new Region();

    long u, v;
    // *** Optimization ***
    // right now this insertion algorithm creates wide horizontal
    // atlases.  It seems it would be more optimal to create more
    // 'square' atlases; or even better insert in such a way as
    // to minimize the total number of used regions, i.e., 
    // always try to attach to one or more edges of existing regions.
    //
    // loop in v: slide test-region vertically across atlas
    for (v = 0; (v+1)*pTexture->GetHeight() <= mpAtlas->GetHeight(); ++v)
    {
        pTest->mTop    = v     * pTexture->GetHeight();
        pTest->mBottom = (v+1) * pTexture->GetHeight();

        // loop in u: slide test-region horizontally across atlas
        for (u = 0; (u+1)*pTexture->GetWidth() <= mpAtlas->GetWidth(); ++u)
        {
            pTest->mLeft  = u     * pTexture->GetWidth();
            pTest->mRight = (u+1) * pTexture->GetWidth();

            // go through all Used regions and see if they overlap
            Region const *pIntersection = Intersects(*pTest);
            if (pIntersection != NULL)
            {
                // found an intersecting used region: try next position
                // but actually advance position by the larger of pTexture's width
                // and this intersection
                float ratio =   static_cast<float>(pIntersection->mRight)
                              / static_cast<float>(pTest->mRight);
                ratio = ceilf(max(0.0f, ratio-1.0f));  
                u += static_cast<long>(ratio);
            }
            else
            {
                // no intersection found: 
                // merge this region into the used region's vector
                Merge(pTest);

                mTotalFreeTexels -= pTexture->GetNumTexels();
                assert(mTotalFreeTexels >= 0);
                CopyBits(*pTest, pTexture->GetD3DTexture());

                // also update the Texture2D object to set correct 
                // atlas pointers and offsets
                OffsetStructure offset;
                offset.uOffset = pTest->mLeft;
                offset.vOffset = pTest->mTop;
                offset.width   = pTest->GetWidth();
                offset.height  = pTest->GetHeight();
                offset.slice   = 0L;
                pTexture->SetAtlas(mpAtlas, offset);

                return true;
            }
        }
    }
    // could not insert: free allocated region and return failure
    delete pTest;
    return false;
}

//-----------------------------------------------------------------------------
// Name: CopyBits()
// Desc: copy the contents of all mip-maps of the passed in texture into 
//       the mpAtlas bits at the offsets indicated by target region.
//-----------------------------------------------------------------------------
void Packer2D::CopyBits(Region const &target, IDirect3DTexture9 *pTexture)
{
    HRESULT         hr;
    RECT            srcRect,       dstRect;
    D3DLOCKED_RECT  srcLockedRect, dstLockedRect;

    srcRect.left = srcRect.top = 0;

    // If -nomipmap was set then mpAtlas only has one mip-map and kNumMipMaps is 1.
    // If it wasn't then mpAtlas has more mip-maps then the texture and kNumMipMaps
    // is the same as there are mip-maps in pTexture.
    int const kNumMipMaps = min(mpAtlas->GetD3DTexture()->GetLevelCount(), pTexture->GetLevelCount());
    if (kNumMipMaps > mMaxNumberMipLevels)
        mMaxNumberMipLevels = kNumMipMaps;
    for (long mipLevel = 0; mipLevel < kNumMipMaps; ++mipLevel)
    {
        long const div        = static_cast<long>(pow((double) 2L, (int) mipLevel));
        long const mipWidth   = max(1L, target.GetWidth() /div);
        long const mipHeight  = max(1L, target.GetHeight()/div);

        srcRect.right  = mipWidth;
        srcRect.bottom = mipHeight;

        dstRect.left   = max(0L, target.mLeft/div);
        dstRect.top    = max(0L, target.mTop /div);
        dstRect.right  = dstRect.left + mipWidth;
        dstRect.bottom = dstRect.top  + mipHeight;

        // These calls to LockRect fail (generate errors:
        // Direct3D9: (ERROR) :Rects for DXT surfaces must be on 4x4 boundaries) 
        // in the 2003 Summer SDK Debug Runtime.  They work just fine 
        // (and as expected) when using the retail run-time: 
        // please make sure to use the retail run-time when running this!
        hr = pTexture->LockRect( mipLevel, &srcLockedRect, &srcRect, D3DLOCK_READONLY );
        assert(hr == S_OK);
        hr = mpAtlas->GetD3DTexture()->LockRect( mipLevel, &dstLockedRect, &dstRect, 0 );
        assert(hr == S_OK);

	    int const kBytesPerRow = (mipWidth * SizeOfTexel(mpAtlas->GetFormat()))/8;
	    int const kBlockFactor = (IsDXTnFormat(mpAtlas->GetFormat()) ? 4 : 1);
        UCHAR *srcPtr = reinterpret_cast<UCHAR*>(srcLockedRect.pBits);
        UCHAR *dstPtr = reinterpret_cast<UCHAR*>(dstLockedRect.pBits);

        for (int i = 0; i < mipHeight/kBlockFactor; ++i)
        {
            memcpy( dstPtr, srcPtr, kBlockFactor * kBytesPerRow );
            srcPtr += srcLockedRect.Pitch;
            dstPtr += dstLockedRect.Pitch;
        }

        hr = pTexture->UnlockRect( mipLevel );
        assert(hr == S_OK);
        hr = mpAtlas->GetD3DTexture()->UnlockRect( mipLevel );
        assert(hr == S_OK);
    }
}

//-----------------------------------------------------------------------------
// Name: Merge()
// Desc: *** Optimization ***
//       Ideally here we should try to merge used region into 
//       fewer and larger regions, ie can the new region combine 
//       with any existing one, if yes, merge them and repeat
//       until no more merges occur.
//-----------------------------------------------------------------------------
void Packer2D::Merge(Region * pNewRegion) 
{
    mUsedRegions.push_back(pNewRegion);
    // since we have the 'advance more than 1 step in u if possible' optimization
    // it makes sense to favor wider over higher regions.
    // so try to merge horizontally before trying to merge vertically.
    ;
}

//-----------------------------------------------------------------------------
// Name: Intersects()
// Desc: returns NULL if the passed in region does not intersect any stored 
//       used regions.
//       Returns a pointer to the intersecting region if it does intersect. 
//-----------------------------------------------------------------------------
Region const * Packer2D::Intersects(Region const &region) const
{
    // go through all Used regions and see if they overlap
    std::vector<Region *>::const_iterator iterUsed;
    for (iterUsed = mUsedRegions.begin(); iterUsed != mUsedRegions.end(); ++iterUsed)
        if ((*iterUsed)->Intersect(region))
        {
            // found an intersecting used region: return the result
            return *iterUsed;
        }

    // made it through w/o finding intersection: return NULL;
    return NULL;
}

//-----------------------------------------------------------------------------
// Name: PackerVolume()
// Desc: Constructor
//-----------------------------------------------------------------------------
PackerVolume::PackerVolume(AtlasVolume * pAtlas)
    : mpAtlas(pAtlas)
    , mSlicesUsed(0)
{
    assert(pAtlas != NULL);
    mMaxNumberMipLevels = 1;    // by definition
}

//-----------------------------------------------------------------------------
// Name: PackerVolume()
// Desc: Destructor
//-----------------------------------------------------------------------------
PackerVolume::~PackerVolume()
{
    ;
}

//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Insert passed in texture into volume texture, return true if 
//       successful
//-----------------------------------------------------------------------------
bool PackerVolume::Insert(Texture2D *pTexture)
{
    if (   (pTexture->GetWidth()  != mpAtlas->GetWidth())
        || (pTexture->GetHeight() != mpAtlas->GetHeight()))
        return false;

    if (mSlicesUsed >= mpAtlas->GetDepth())
        return false;

    CopyBits(pTexture->GetD3DTexture());

    // also update the Texture2D object to set correct 
    // atlas pointers and offsets
    OffsetStructure offset;
    offset.uOffset = 0;
    offset.vOffset = 0;
    offset.width   = pTexture->GetWidth();
    offset.height  = pTexture->GetHeight();
    offset.slice   = mSlicesUsed;
    pTexture->SetAtlas(mpAtlas, offset);

    ++mSlicesUsed;
    return true;
}

//-----------------------------------------------------------------------------
// Name: CopyBits()
// Desc: Copy the bits of the passed in texture the indicated slice of the 
//       volume texture;
//-----------------------------------------------------------------------------
void PackerVolume::CopyBits(IDirect3DTexture9 *pTexture)
{
    D3DLOCKED_RECT  srcLockedRect;
    HRESULT hr = pTexture->LockRect( 0, &srcLockedRect, NULL, D3DLOCK_READONLY );
    assert(hr == S_OK);

    D3DLOCKED_BOX   dstLockedBox;
    hr = mpAtlas->GetD3DTexture()->LockBox(0, &dstLockedBox, NULL, D3DLOCK_DISCARD);
    assert(hr == S_OK);

	int const kBytesPerRow = (mpAtlas->GetWidth() * SizeOfTexel(mpAtlas->GetFormat()))/8;
	int const kBlockFactor = (IsDXTnFormat(mpAtlas->GetFormat()) ? 4 : 1);
    UCHAR *srcPtr = reinterpret_cast<UCHAR*>(srcLockedRect.pBits);
    UCHAR *dstPtr = reinterpret_cast<UCHAR*>(dstLockedBox.pBits);

    dstPtr += (mSlicesUsed * dstLockedBox.SlicePitch);

    for (int i = 0; i < mpAtlas->GetHeight()/kBlockFactor; ++i)
    {
        memcpy( dstPtr, srcPtr, kBlockFactor * kBytesPerRow );
        srcPtr += srcLockedRect.Pitch;
        dstPtr += dstLockedBox.RowPitch;
    }

    hr = pTexture->UnlockRect( 0 );
    assert(hr == S_OK);
    hr = mpAtlas->GetD3DTexture()->UnlockBox( 0 );
    assert(hr == S_OK);
}

//-----------------------------------------------------------------------------
// Name: CopyBits()
// Desc: Copy the bits of the passed in volume texture into the 
//       volume texture;
//-----------------------------------------------------------------------------
void PackerVolume::CopyBits(IDirect3DVolumeTexture9 *pVolumeTexture)
{
    D3DLOCKED_BOX   srcLockedBox;
    HRESULT hr = pVolumeTexture->LockBox(0, &srcLockedBox, NULL, D3DLOCK_READONLY);
    assert(hr == S_OK);

    D3DLOCKED_BOX   dstLockedBox;
    hr = mpAtlas->GetD3DTexture()->LockBox(0, &dstLockedBox, NULL, D3DLOCK_DISCARD);
    assert(hr == S_OK);

	int const kBytesPerSlice = (mpAtlas->GetWidth() * mpAtlas->GetHeight() * SizeOfTexel(mpAtlas->GetFormat()))/8;
	int const kBlockFactor = (IsDXTnFormat(mpAtlas->GetFormat()) ? 4 : 1);
    UCHAR *srcPtr = reinterpret_cast<UCHAR*>(srcLockedBox.pBits);
    UCHAR *dstPtr = reinterpret_cast<UCHAR*>(dstLockedBox.pBits);

    // Format, width, and height of these two volumes are assumed to be the same
    // between these two textures...
    assert(srcLockedBox.RowPitch == dstLockedBox.RowPitch);
    
    for (int slice = 0; slice < mpAtlas->GetDepth(); ++slice)
    {
        memcpy( dstPtr, srcPtr, kBlockFactor * kBytesPerSlice );
        srcPtr += srcLockedBox.SlicePitch;
        dstPtr += dstLockedBox.SlicePitch;
    }
    hr = pVolumeTexture->UnlockBox( 0 );
    assert(hr == S_OK);
    hr = mpAtlas->GetD3DTexture()->UnlockBox( 0 );
    assert(hr == S_OK);
}

//-----------------------------------------------------------------------------
// Name: GetSlicesUsed()
// Desc: Returns how many slices in the valome are actually used so far.
//-----------------------------------------------------------------------------
int PackerVolume::GetSlicesUsed() const
{
    return mSlicesUsed;
}

