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
// File: AtlasContainer.h
// Desc: Header file for AtlasContainer class
//-----------------------------------------------------------------------------
#ifndef ATLASCONTAINER_H
#define ATLASCONTAINER_H

#include "TATypes.h"

class CmdLineOptionCollection;

//-----------------------------------------------------------------------------
// Name: AtlasContainer
// Desc: Simple class that encapsulates global operations on all existing 
//       atlases, e.g., freeing all memory used by the various atlas objects,
//       inserting a vector of the same format into a corresponding 
//       vector of atlases, shrinking all stored atlases into min size,
//       writing their data to disk etc.
//-----------------------------------------------------------------------------
class AtlasContainer
{
public:
    AtlasContainer(CmdLineOptionCollection const &options, int numFormats);
    ~AtlasContainer();

    void Insert(int i, TTexture2DPtrVector const &textureVector);
    void Shrink();
    void WriteToDisk() const;

//@{ Jaewon 200411202
//private:
//@} Jaewon
    CmdLineOptionCollection const * mpOptions;
    int                             mNumFormats;
    TAtlasVector *                  mpAtlasVectorArray;
};


#endif TEXTUREOBJECT_H
