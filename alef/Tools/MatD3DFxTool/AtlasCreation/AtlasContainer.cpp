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
// File: AtlasContainer.cpp
// Desc: Implementation of AtlasContainer
//-----------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>
#include "AtlasContainer.h"
#include "CmdLineOptions.h"
#include "TextureObject.h"

//-----------------------------------------------------------------------------
// Name: AtlasContainer()
// Desc: Constructor for class: set everything to good defaults 
//-----------------------------------------------------------------------------
AtlasContainer::AtlasContainer(CmdLineOptionCollection const &options, int numFormats)
    : mNumFormats(numFormats)
    , mpOptions(&options)
    , mpAtlasVectorArray(NULL)
{
    mpAtlasVectorArray = new TAtlasVector[numFormats];
}

//-----------------------------------------------------------------------------
// Name: AtlasContainer()
// Desc: Destructor for class: free all memory/clean up
//-----------------------------------------------------------------------------
AtlasContainer::~AtlasContainer()
{
    TAtlasVector::const_iterator    atlas;

    for (int i = 0; i < mNumFormats; ++i)
        for (atlas = mpAtlasVectorArray[i].begin(); atlas != mpAtlasVectorArray[i].end(); ++atlas)
            delete (*atlas);

    delete [] mpAtlasVectorArray;
}

//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Insert all textures in the passed in vector into the i-th atlas vector
//       Update the passed in textures to point at their relevant atlases:
//       This modifies the pointed at data, but the vector in fact stays const.
//-----------------------------------------------------------------------------
void AtlasContainer::Insert(int i, TTexture2DPtrVector const &textureVector)
{
    // for each texture in the vector
    int     totalNumAtlases = 0;
    TTexture2DPtrVector::const_iterator   texIter;
    for (texIter = textureVector.begin(); texIter != textureVector.end(); ++texIter)  
    {
        // try inserting this texture into all existing elements of the atlas vector:
        // if they all fail (ie they are full), then create a new atlas and insert it there.
        TAtlasVector::iterator    atlas;
        for (atlas = mpAtlasVectorArray[i].begin(); atlas != mpAtlasVectorArray[i].end(); ++atlas)   
            if ((*atlas)->Insert(*texIter))
                break;
        if (atlas == mpAtlasVectorArray[i].end())       // texture was not inserted anywhere...
        {
            if (mpOptions->IsSet(CLO_VOLUME))
            {
                AtlasVolume *    pVolumeAtlas = new AtlasVolume(*mpOptions, *texIter, totalNumAtlases);
                mpAtlasVectorArray[i].push_back(pVolumeAtlas);
                ++totalNumAtlases;
            }
            else if (false)
            {
                AtlasCube *    pCubeAtlas = new AtlasCube(*mpOptions, *texIter, totalNumAtlases);
                mpAtlasVectorArray[i].push_back(pCubeAtlas);
                ++totalNumAtlases;
            }
            else
            {
                Atlas2D *    p2DAtlas = new Atlas2D(*mpOptions, *texIter, totalNumAtlases);
                mpAtlasVectorArray[i].push_back(p2DAtlas);
                ++totalNumAtlases;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Name: Shrink()
// Desc: Go through all allocated atlases and attempt to reduce their size
//       w/o losing data.  If possible, createa new texture corresponding 
//       to the new size, copy bits over and free the old one.
//-----------------------------------------------------------------------------
void AtlasContainer::Shrink() 
{
    TAtlasVector::iterator    atlas;
    for (int i = 0; i < mNumFormats; ++i)
        for (atlas = mpAtlasVectorArray[i].begin(); atlas != mpAtlasVectorArray[i].end(); ++atlas)   
            (*atlas)->Shrink();
}

//-----------------------------------------------------------------------------
// Name: WriteToDisk()
// Desc: Write all atlases stored in this container onto their respective 
//       files on-disk.
//-----------------------------------------------------------------------------
void AtlasContainer::WriteToDisk() const
{
    TAtlasVector::const_iterator    atlas;
    for (int i = 0; i < mNumFormats; ++i)
        for (atlas = mpAtlasVectorArray[i].begin(); atlas != mpAtlasVectorArray[i].end(); ++atlas)   
            (*atlas)->WriteToDisk();
}

