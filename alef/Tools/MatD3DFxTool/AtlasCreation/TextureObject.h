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
// File: TextureObject.h
// Desc: Header file for TextureObject class
//-----------------------------------------------------------------------------
#ifndef TEXTUREOBJECT_H
#define TEXTUREOBJECT_H


#include <d3dx9.h>
#include "TATypes.h"

class CmdLineOptionCollection;
class Packer2D;
class PackerVolume;

//-----------------------------------------------------------------------------
// Name: OffsetStructure
// Desc: Simple structure that is to indicate where in an atlas a texture
//       was stored.
//-----------------------------------------------------------------------------
struct OffsetStructure
{
    long   uOffset;
    long   vOffset;
    long   width;
    long   height;
    long   slice;
};

//-----------------------------------------------------------------------------
// Name: TextureObject
// Desc: Pure virtual base class to store texture objects.  The textures
//       that need to be inserted into atlases, as well as the different
//       types of atlases derive from this class.
//-----------------------------------------------------------------------------
class TextureObject
{
public:
    enum eTextureType
    {
        TEXTYPE_UNINITIALIZED = 0,
        TEXTYPE_2D,
        TEXTYPE_ATLAS2D,
        TEXTYPE_ATLASVOLUME,
        TEXTYPE_ATLASCUBE,
        TEXTYPE_NUM,
    };

public:
    TextureObject();
    ~TextureObject();

    virtual D3DFORMAT   GetFormat()                  const = 0;
    virtual bool        IsSupportedFormat(D3DFORMAT) const = 0;
    virtual long        GetNumTexels()               const = 0;

    IDirect3DDevice9 *  GetDevice()   const;
    char const *        GetFilename() const;
    eTextureType        GetType()     const;

    void Init(IDirect3DDevice9 *pD3d, char const *pFilename);

protected:
    void PrintError(char const * pText) const;

protected:
    IDirect3DDevice9 *          mpD3DDev;
    char const *                mpFilename;
    eTextureType                mType;
};

//-----------------------------------------------------------------------------
// Name: Texture2D
// Desc: Derived class to store the textures to be inserted into atlases.
//       that need to be inserted into atlases, as well as the different
//       types of atlases derive from this class.
//-----------------------------------------------------------------------------
class AtlasObject;
class Texture2D : public TextureObject
{
public:
    Texture2D();
    ~Texture2D();

    virtual D3DFORMAT   GetFormat()                  const;
    virtual bool        IsSupportedFormat(D3DFORMAT) const;
    virtual long        GetNumTexels()               const;
    
    virtual long        GetWidth()  const;
    virtual long        GetHeight() const;

    HRESULT             LoadTexture(CmdLineOptionCollection const &options);
    void                SetAtlas(AtlasObject const *pAtlas, OffsetStructure const &offset);

    IDirect3DTexture9*  GetD3DTexture()                                                const;
    void                WriteTAILine(CmdLineOptionCollection const &options, FILE *fp) const;
	//@{ Jaewon 20041202
	const char *		GetTAI(CmdLineOptionCollection const &options, 
							   float& uOffset, float& vOffset, float& uWidth, float& vHeight) const;
	//@} Jaewon

private:
    IDirect3DTexture9*          mpTexture2D;
    AtlasObject const *         mpAtlas;
    OffsetStructure             mOffset;
};

//-----------------------------------------------------------------------------
// Name: AtlasObject
// Desc: Derived class that is pure virtual base class to the different 
//       atlas types.
//-----------------------------------------------------------------------------
class AtlasObject : public TextureObject
{
public:
    AtlasObject();
    virtual ~AtlasObject();

    virtual D3DFORMAT   GetFormat()                  const = 0;
    virtual bool        IsSupportedFormat(D3DFORMAT) const = 0;
    virtual long        GetNumTexels()               const = 0;

    virtual bool Insert(Texture2D *pTexture) = 0;
    virtual void Shrink();
    virtual void WriteToDisk() const = 0;
    virtual long GetWidth()    const = 0;
    virtual long GetHeight()   const = 0;

    int          GetId()       const;
    char const * GetFilename() const;

protected:
    int         mAtlasId;
    char        mFilename[kFilenameLength];
};

//-----------------------------------------------------------------------------
// Name: Atlas2D
// Desc: Derived class representing 2D Atlases.
//       Most notably it stores a pointer to a Packer2D object so it 
//       knows how to deal w/ insertions.
//-----------------------------------------------------------------------------
class Atlas2D : public AtlasObject
{
public:
    Atlas2D(CmdLineOptionCollection const &options, Texture2D *pTexture, int num);
    virtual ~Atlas2D();

    virtual D3DFORMAT   GetFormat()                  const;
    virtual bool        IsSupportedFormat(D3DFORMAT) const;
    virtual long        GetNumTexels()               const;

    virtual bool        Insert(Texture2D *pTexture);
    virtual void        Shrink();
    virtual void        WriteToDisk() const;
    virtual long        GetWidth()    const;
    virtual long        GetHeight()   const;

    IDirect3DTexture9*  GetD3DTexture() const;

private:
    IDirect3DTexture9*          mpTexture2D;
    Packer2D *                  mpPacker2D;
};

//-----------------------------------------------------------------------------
// Name: AtlasVolume
// Desc: Derived class representing Volume Atlases.
//       Not yet writen: this is just a shell
//-----------------------------------------------------------------------------
class AtlasVolume : public AtlasObject
{
public:
    AtlasVolume(CmdLineOptionCollection const &options, Texture2D *pTexture, int num);
    virtual ~AtlasVolume();

    virtual D3DFORMAT   GetFormat()                  const;
    virtual bool        IsSupportedFormat(D3DFORMAT) const;
    virtual long        GetNumTexels()               const;

    virtual bool        Insert(Texture2D *pTexture);
    virtual void        Shrink();
    virtual void        WriteToDisk() const;
    virtual long        GetWidth()    const;
    virtual long        GetHeight()   const;
            long        GetDepth()   const;

    IDirect3DVolumeTexture9*  GetD3DTexture() const;

private:
    IDirect3DVolumeTexture9*    mpTextureVolume;
    PackerVolume *              mpPackerVolume;
};

//-----------------------------------------------------------------------------
// Name: AtlasCube
// Desc: Derived class representing Volume Atlases.
//       Not yet written: this is just a shell
//-----------------------------------------------------------------------------
class AtlasCube : public AtlasObject
{
public:
    AtlasCube(CmdLineOptionCollection const &options, Texture2D *pTexture, int num);
    virtual ~AtlasCube();

    virtual D3DFORMAT   GetFormat()                  const;
    virtual bool        IsSupportedFormat(D3DFORMAT) const;
    virtual long        GetNumTexels()               const;

    virtual bool        Insert(Texture2D *pTexture);
    virtual void        WriteToDisk() const;
    virtual long        GetWidth()    const;
    virtual long        GetHeight()   const;

private:
    IDirect3DCubeTexture9*    mpTextureCube;
};

//-----------------------------------------------------------------------------
// Name: Texture2DGreater
// Desc: struct used for sorting of Texture2D objects
//-----------------------------------------------------------------------------
typedef struct _TEXTURE2DGREATER 
{
    bool operator()(Texture2D const *s1, Texture2D const *s2) const
    {
        if (s1->GetHeight()*s1->GetWidth() > s2->GetHeight()*s2->GetWidth())
            return true;
        else if (   (s1->GetHeight() > s2->GetHeight())
                 && (s1->GetHeight()*s1->GetWidth() == s2->GetHeight()*s2->GetWidth()))
            return true;
        else if (   (s1->GetWidth()  > s2->GetWidth())
                 && (s1->GetHeight()*s1->GetWidth() == s2->GetHeight()*s2->GetWidth()))
            return true;
        else 
            return false;
    }
} Texture2DGreater;



#endif TEXTUREOBJECT_H
