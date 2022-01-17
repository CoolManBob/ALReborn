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
// File: Packer.h
// Desc: Header file Packer class
//-----------------------------------------------------------------------------

#ifndef PACKER_H
#define PACKER_H

#include <d3dx9.h>
#include "TATypes.h"

class CmdLineOptionCollection;
class Texture2D;
class Atlas2D;
class AtlasVolume;

//-----------------------------------------------------------------------------
// Name: Region
// Desc: Simple calss representing a rectangle. It knows how to intersect 
//       against other such rectangles.
//       This class is used to store dirty (used) rectangles representing 
//       areas in a texture that valid data has been copied into.
//       A collection of these thus shows which texels are not 
//       available for storing new information (and thus which texels
//       *are* available).
//-----------------------------------------------------------------------------
class Region
{
public:
    Region();
    ~Region();

    long GetWidth() const;
    long GetHeight() const;
    bool Intersect(Region const &region) const; 

public:
    long     mLeft;
    long     mRight;
    long     mTop;
    long     mBottom;
};

//-----------------------------------------------------------------------------
// Name: Packer
// Desc: Pure virtual base class for Packer objects.  For example, a Packer2D
//       object knows how to insert new 2D textures into a 2D atlas.
//       The base class defines the common entry points and some house-keeping 
//       data
//-----------------------------------------------------------------------------
class Packer
{
public:
    Packer();
    ~Packer();

    virtual bool Insert(Texture2D *pTexture) = 0; 

    int GetMaxMipLevels() const;

protected:
    int  SizeOfTexel (D3DFORMAT format) const;
    bool IsDXTnFormat(D3DFORMAT format) const;

protected:
    int     mTotalFreeTexels;
    int     mMaxNumberMipLevels;
};

//-----------------------------------------------------------------------------
// Name: Packer2D
// Desc: Derived class that knows how to deal with Atlas2D objects,
//       specifically how to insert Texture2D objects into Atlas2D objects
//-----------------------------------------------------------------------------
class Packer2D : public Packer
{
public:
    Packer2D(Atlas2D * pAtlas);
    ~Packer2D();

    virtual bool Insert(Texture2D *pTexture); 

    Region const * Intersects(Region const &region) const;
    void           CopyBits(Region const &test, IDirect3DTexture9 *pTexture);

private:
    void Merge(Region * pNewRegion);

private:
    Atlas2D *               mpAtlas;
    std::vector<Region *>   mUsedRegions;

};

//-----------------------------------------------------------------------------
// Name: PackerVolume
// Desc: Derived class that knows how to deal with AtlasVolume objects,
//       specifically how to insert Texture2D objects into AtlasVolume objects
//-----------------------------------------------------------------------------
class PackerVolume : public Packer
{
public:
    PackerVolume(AtlasVolume * pAtlas);
    ~PackerVolume();

    virtual bool Insert(Texture2D *pTexture); 
    void         CopyBits(IDirect3DTexture9       *pTexture);
    void         CopyBits(IDirect3DVolumeTexture9 *pVolumeTexture);
    int          GetSlicesUsed() const;

private:
    AtlasVolume *mpAtlas;
    int          mSlicesUsed;

};

#endif // PACKER_H

