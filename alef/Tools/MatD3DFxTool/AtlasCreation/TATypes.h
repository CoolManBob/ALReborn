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
// File: TATypes.h
// Desc: Definition of various types usied in the texture atlas project
//-----------------------------------------------------------------------------

#ifndef TATYPES_H
#define TATYPES_H

#include <map>
#include <vector>

#include <d3d9.h> 


class Texture2D;
class AtlasObject;

enum {
    kFilenameLength    = 256,
    kPrintStringLength = 512,
};

typedef std::vector<Texture2D *>                          TTexture2DPtrVector;
typedef std::vector<AtlasObject *>                        TAtlasVector;
typedef std::map <D3DFORMAT, TTexture2DPtrVector >        TNewFormatMap;

#endif // TATYPES_H
