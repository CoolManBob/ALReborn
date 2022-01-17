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
// File: CmdLineOptions.cpp
// Desc: Implementation of CmdLineOptions
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "CmdLineOptions.h"
#include "TATypes.h"

//-----------------------------------------------------------------------------
// Name: CmdLineOptionCollection()
// Desc: Constructor for class: parses based on passed in args
//-----------------------------------------------------------------------------
CmdLineOptionCollection::CmdLineOptionCollection(int argc, char **argv)
    : mbValid (false)
    , mNumFilenames(0)
    , mpFilenames(NULL)
{
    // reset everything to sensible values
    for (int i = 0; i < CLO_NUM; ++i)
    {
        mCurrent[i].present    = false;
        mCurrent[i].pStartArgs = NULL;
    }

    mbValid = Parse(argc, argv);
}

//-----------------------------------------------------------------------------
// Name: ~CmdLineOptionCollection()
// Desc: Destructor for class: free all memory
//-----------------------------------------------------------------------------
CmdLineOptionCollection::~CmdLineOptionCollection()
{
    // nothing to dealloc/free
    mbValid = false;   
}

//-----------------------------------------------------------------------------
// Name: IsValid()
// Desc: Returns the content of internal variable that indicates whether 
//       previsous (internal) calls to check() (see below) were successful
//-----------------------------------------------------------------------------
bool    CmdLineOptionCollection::IsValid() const
{
    return mbValid;
}

//-----------------------------------------------------------------------------
// Name: IsSet()
// Desc: returns true if the passed in option is set and valid.
//-----------------------------------------------------------------------------
bool    CmdLineOptionCollection::IsSet(eCmdLineOptionType option) const
{
    if (option < CLO_NUM)
        return mCurrent[option].present;
    return false;
}

//-----------------------------------------------------------------------------
// Name: GetArgument()
// Desc: Returns the value of the argument of the passed in option
//-----------------------------------------------------------------------------
char *  CmdLineOptionCollection::GetArgument(eCmdLineOptionType option, int n) const
{
    if ((option < CLO_NUM) && (n < kNumArguments[option]))
        return mCurrent[option].pStartArgs[n];
    return NULL;
}

//-----------------------------------------------------------------------------
// Name: GetNumFilenames()
// Desc: Returns the number of filenames 
//-----------------------------------------------------------------------------
int     CmdLineOptionCollection::GetNumFilenames() const
{
    return mNumFilenames;
}

//-----------------------------------------------------------------------------
// Name: GetFilenames()
// Desc: Returns the filenames
//-----------------------------------------------------------------------------
void CmdLineOptionCollection::GetFilename(int i, char const **ppFilename) const
{
    assert(i >= 0);
    assert(i < mNumFilenames);
    *ppFilename = mpFilenames[i];
}

//-----------------------------------------------------------------------------
// Name: Parse()
// Desc: Parses the passed in argc/argv argumetns for all available cmd-line
//       options and sets internal variables accordingly.
//       Returns true if no errors occured.
//-----------------------------------------------------------------------------
bool    CmdLineOptionCollection::Parse(int argc, char **argv)
{
    // loop through all arguments:
    // if a known cmd-line argument, set internal bits accordingly
    // if unknown, assume it is a texture-filename, switch into 
    // bTextureNamesMode and count these arguments as filenames.
    bool bTextureNamesMode = false;

    int i, j;
    for (i = 1; i < argc; ++i)          // the very first argument is the exe-name: skip that one
        if (bTextureNamesMode)
        {
            ++mNumFilenames;            // just count how many texture files there are
        }
        else
        {
            for (j = 0; j < CLO_NUM; ++j)
                if (!strcmpi(kParseString[j], argv[i]))         // is it a known cmd-line option?
                {
                    if (mCurrent[j].present)                    //yes: was it set before?
                    {
                        char warning[kPrintStringLength]; 
                        sprintf( warning, "Option \'%s\' specified multiple times on the cmd-line:",
                                          kShortDescription[static_cast<eCmdLineOptionType>(j)]);
                        PrintWarning(warning);
                        PrintWarning("         Only keeping the last value." );
                    }
                    mCurrent[j].present = true;                 //set the according bit


                    if (kNumArguments[j] > 0)                   // does this option have arguments?
                    {
                        mCurrent[j].pStartArgs = &(argv[i+1]);  // yes: store them
                        i += kNumArguments[j];                  // and advance to the next option
                    }
                    break;
                }

            if (j == CLO_NUM)                                   // Did not recognize this option:
            {
                bTextureNamesMode = true;                       // assume we are now processing texture filenames
                mNumFilenames     = 1;
                mpFilenames       = &(argv[i]);
            }
        }

    // Done parsing: check parsed values for internal consistency
    return Check();
}

//-----------------------------------------------------------------------------
// Name: IsArgPowerOf2()
// Desc: Return true if the first argument of the passed in option is a valid 
//       integer greater than 1 and a power of 2.  If not print error and 
//       return false.
//-----------------------------------------------------------------------------
bool CmdLineOptionCollection::IsArgPowerOf2(eCmdLineOptionType option) const
{
    char        string[kPrintStringLength];
    int         resolution;    
    int const   kError = sscanf(mCurrent[option].pStartArgs[0], "%i", &resolution);
    
    if ((kError != 1) || (resolution < 2))
    {
        sprintf(string, "%s option requires argument to be an integer greater than 1.", kShortDescription[option]);
        return PrintError(string);
    }

    // compute remainder of log2(resolution): 
    // if it is 0 then resolution is a power of 2, and thus a legal argument 
    float const kRemainder = fmodf(logf(static_cast<float>(resolution))/logf(2.0f), 1.0f);
    if (kRemainder != 0.0f)
    {
        sprintf(string, "%s option requires argument to be a power of 2.", kShortDescription[option]);
        return PrintError(string);
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name: Check()
// Desc: Checks that the currently stored values are consistent and valid.
//       If not prints an error and returns false.
//-----------------------------------------------------------------------------
bool    CmdLineOptionCollection::Check() const
{
    char string[kPrintStringLength];

    // Make sure at least one texture file was passed in:
    if (mNumFilenames < 1)
        return PrintError("No texture filenames specified.");

    // check that if width/height/depth is given 
    // 1) it is a valid number
    // 2) that the argument is a power of two greater than 1.

    // if -width or -height is given w/ -volume: warn that we ignore these for now
    if (mCurrent[CLO_WIDTH].present && (mCurrent[CLO_VOLUME].present))
    {
        sprintf(string, "%s currently uses the passed in texture's width and thus ignores %s", 
                kShortDescription[CLO_VOLUME], kShortDescription[CLO_WIDTH]);
        PrintWarning(string);
    } 
    else if (mCurrent[CLO_WIDTH].present && (! IsArgPowerOf2(CLO_WIDTH)))
        return false;

    if (mCurrent[CLO_HEIGHT].present && (mCurrent[CLO_VOLUME].present))
    {
        sprintf(string, "%s currently uses the passed in texture's height and thus ignores %s", 
                kShortDescription[CLO_VOLUME], kShortDescription[CLO_HEIGHT]);
        PrintWarning(string);
    } 
    else if (mCurrent[CLO_HEIGHT].present && (! IsArgPowerOf2(CLO_HEIGHT)))
        return false;

    // if -depth is given w/o -volume: warn that we ignore -depth 
    // and skip the argument test for depth
    if (mCurrent[CLO_DEPTH].present && (! mCurrent[CLO_VOLUME].present))
    {
        sprintf(string, "%s specified, yet %s is not specified: ignoring %s", 
                kShortDescription[CLO_DEPTH], kShortDescription[CLO_VOLUME], kShortDescription[CLO_DEPTH]);
        PrintWarning(string);
    }
    if (mCurrent[CLO_DEPTH].present && mCurrent[CLO_VOLUME].present && (! IsArgPowerOf2(CLO_DEPTH)))
        return false;


    // Make sure that if -volume is given -nomipmap is also on
    if (mCurrent[CLO_VOLUME].present && !mCurrent[CLO_NOMIPMAP].present)
    {
        sprintf(string, "%s option requires %s option to be set.", kShortDescription[CLO_VOLUME], kShortDescription[CLO_NOMIPMAP]);
        return PrintError(string);
    }

    // Make sure that an outfilename was given
    if (! mCurrent[CLO_OUTFILE].present)
    {
        sprintf(string, "%s option has to be set.", kShortDescription[CLO_OUTFILE]);
        return PrintError(string);
    }

    // if help flag was given then print usage
    if (   mCurrent[CLO_HELP].present 
        || mCurrent[CLO_HELP_ALTERNATE1].present 
        || mCurrent[CLO_HELP_ALTERNATE2].present)
        PrintUsage();

    return true;
}

//-----------------------------------------------------------------------------
// Name: PrintUsage()
// Desc: Prints correct usage info
//       Prints to stderr.
//-----------------------------------------------------------------------------
void CmdLineOptionCollection::PrintUsage() const
{
    int i;

	fprintf( stderr, "\n");
	fprintf( stderr, "Usage: TextureAtlasTool.exe");    
    for (i = 0; i < CLO_NUM; ++i)
        fprintf( stderr, " %s", kShortDescription[i]);
    fprintf( stderr, " <img1> <img2> ...\n");

    for (i = 0; i < CLO_NUM; ++i)
	    fprintf( stderr, "   %-13s   %s\n", kShortDescription[i], kDescription[i]);
    fprintf( stderr, "\n" );
    fprintf( stderr, "For example: TextureAtlasTool.exe -halftexel -o Default Textures\\*.png\n\n" );
}

//-----------------------------------------------------------------------------
// Name: PrintError()
// Desc: Prints the passed in string as an error and returns false.
//-----------------------------------------------------------------------------
bool    CmdLineOptionCollection::PrintError(char const * string) const
{
    fprintf( stderr, "*** Error: %s\n", string);
    PrintUsage();
    return false;
}

//-----------------------------------------------------------------------------
// Name: PrintWarning()
// Desc: Prints the passed in string as a warning.
//-----------------------------------------------------------------------------
void    CmdLineOptionCollection::PrintWarning(char const * string) const
{
    fprintf( stderr, "Warning: %s\n", string);
}

