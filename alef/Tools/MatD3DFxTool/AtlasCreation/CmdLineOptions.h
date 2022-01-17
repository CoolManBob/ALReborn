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
// File: CmdLineOptions.h
// Desc: Header file CmdLineOptions
//-----------------------------------------------------------------------------
#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#define MAX_PARSESTRING_LENGTH       16
#define MAX_SHORTDESCRIPTION_LENGTH  32
#define MAX_DESCRIPTION_LENGTH      256


// To add a command line option, you have to add a line to
// enum CmdLineOptionType, char parsestring, char shortDescription, 
// char description, and int numArguments.
// PrintUsage and parsing will then automatically deal with 
// the extra command line option.

enum eCmdLineOptionType 
{
    CLO_HELP = 0,
    CLO_HELP_ALTERNATE1,
    CLO_HELP_ALTERNATE2,
    CLO_NOMIPMAP,
    CLO_VOLUME,
    CLO_HALFTEXEL,
    CLO_WIDTH,
    CLO_HEIGHT,
    CLO_DEPTH,
    CLO_OUTFILE,
    CLO_NUM,
};

const char kParseString[CLO_NUM][MAX_PARSESTRING_LENGTH] = 
{
    "-h",
    "-help",
    "-?",
    "-nomipmap",
    "-volume",
    "-halftexel",
    "-width",
    "-height",
    "-depth",
    "-o",
};

const char kShortDescription[CLO_NUM][MAX_SHORTDESCRIPTION_LENGTH] = 
{
    "-h",
    "-help",
    "-?",
    "-nomipmap",
    "-volume",
    "-halftexel",
    "-width <w>",
    "-height <h>",
    "-depth <d>",
    "-o <filename>",
};

const char kDescription[CLO_NUM][MAX_DESCRIPTION_LENGTH] = 
{
    "prints this usage help",
    "prints this usage help",
    "prints this usage help",
    "only writes out the top-level mipmap",
    "only valid w/ -nomipmap; make atlases volume textures",
    "adds a half-texel offset to the generated texture coordinates",
    "limits texture atlases to a maximum width of w texels",
    "limits texture atlases to a maximum height of h texels",
    "limits texture atlases to a maximum depth of d slices",
    "mandatory option that specifies output filename",
};

const int kNumArguments[CLO_NUM] = 
{
    0, 
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
};

//-----------------------------------------------------------------------------
// Name: CmdLineOptionCollection
// Desc: Given cmd-line arguments this class parses that info
//       for valid cmd-line argumetns as defined in the arrays
//       eCmdLineOptionType, kParseString, etc.  
//       It will check their argumetns and combinations to be
//       valid and print errors and warnings as appropriate.
//
//       An object of this type can then be passed around and easily 
//       querried whether any argument is set and what its parameters 
//       are.
//-----------------------------------------------------------------------------
class CmdLineOptionCollection
{
public:
    CmdLineOptionCollection(int argc, char **argv);
    ~CmdLineOptionCollection();

    bool    IsValid()                                       const;
    bool    IsSet(eCmdLineOptionType option)                const;
    char *  GetArgument(eCmdLineOptionType option, int n)   const;
    int     GetNumFilenames()                               const;
    void    GetFilename(int i, char const **ppFilename)     const;

private:
    bool Parse(int argc, char **argv);

    bool Check()                                  const;
    bool IsArgPowerOf2(eCmdLineOptionType option) const;

    void PrintUsage()                      const;
    bool PrintError(  char const * string) const;
    void PrintWarning(char const * string) const;

private:
    struct CmdLineOption 
    {
        bool                present;
        char * const *      pStartArgs;
    };

private:
    bool            mbValid;
    CmdLineOption   mCurrent[CLO_NUM];
    int             mNumFilenames;
    char * const *  mpFilenames;
};

#endif CMDLINEOPTIONS_H
