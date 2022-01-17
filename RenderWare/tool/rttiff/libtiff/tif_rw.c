/* $Header: //RenderWare/RW37Active/rwsdk/tool/tiff/tif_rw.c#1 $ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library Win32-specific Routines.  Adapted from tif_unix.c 4/5/95 by
 * Scott Wagner (wagner@itek.com), Itek Graphix, Rochester, NY USA
 */
#include "tiffiop.h"

/*
tiffcomp.h Line 154 defines INLINE in a really stupid way
This screws up with the Sony headers, libdma.h, included by rwcore.h
since it defines functions dozens of times in your library.
So put it back the way Sony would have done it...
*/
#undef INLINE

/* This MUST be included AFTER libtiff or on PS2 we get weird clashing */
#include <rwcore.h>
#include "rtfsyst.h"


static tsize_t
_tiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return ((tsize_t) (RwFread(buf, 1, size, fd)));
}

static tsize_t
_tiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return ((tsize_t) (RwFwrite(buf, 1, size, fd)));
}

static toff_t
_tiffSeekProc(thandle_t fd, toff_t off, int whence)
{
    int pos;

    /* we use this as a special code, so avoid accepting it */
    if( off == 0xFFFFFFFF )
        return 0xFFFFFFFF;

    RwFseek(fd, off, whence);

    pos = RwFtell(fd);

    return pos;
}

static int
_tiffCloseProc(thandle_t fd)
{
    return (RwFclose(fd) ? 0 : -1);
}

static toff_t
_tiffSizeProc(thandle_t fd __RWUNUSED__)
{
/*  return ((toff_t)GetFileSize(fd, NULL)); */
    return 0;
}

static int
_tiffMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
    (void) fd; (void) pbase; (void) psize;
    return (0);
}

static void
_tiffUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
    (void) fd; (void) base; (void) size;
}

/* stream functions */
static tsize_t
_tiffReadStreamProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return ((tsize_t) (RwStreamRead((RwStream *) fd, buf, size)));
}

static tsize_t
_tiffWriteStreamProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return 0;
}

static toff_t
_tiffSeekStreamProc(thandle_t fd, toff_t off, int whence)
{
	RwStream *stream = (RwStream *) fd;

    /* we use this as a special code, so avoid accepting it */
    if( off == 0xFFFFFFFF )

	if (stream->type !=	rwSTREAMMEMORY)
        return 0xFFFFFFFF;

    switch (whence)
    {
        case SEEK_CUR:
			RwStreamSkip(stream, off);
			break;

        case SEEK_END:
			stream->Type.memory.position = stream->Type.memory.nSize - off;
        break;

        case SEEK_SET:
			stream->Type.memory.position = off;
			break;
	}

    return stream->Type.memory.position;
}

static int
_tiffCloseStreamProc(thandle_t fd)
{
    return 0;
}

static toff_t
_tiffSizeStreamProc(thandle_t fd __RWUNUSED__)
{
	RwStream *stream = (RwStream *) fd;

	if (stream->type !=	rwSTREAMMEMORY)
        return 0xFFFFFFFF;

	return stream->Type.memory.nSize;
}

/*
 * Open a TIFF file descriptor for read/writing.
 */
TIFF*
TIFFFdOpen(int fd, const char* name, const char* mode)
{
    TIFF* tif;

    tif = TIFFClientOpen(name, mode,
        (thandle_t) fd,
        _tiffReadProc, _tiffWriteProc,
        _tiffSeekProc, _tiffCloseProc, _tiffSizeProc,
        _tiffMapProc, _tiffUnmapProc);
    if (tif)
        tif->tif_fd = fd;
    return (tif);
}

TIFF*
TIFFStreamOpen(RwStream* stream, const char* name, const char* mode)
{
    TIFF* tif;

    tif = TIFFClientOpen(name, mode,
        (thandle_t) stream,
        _tiffReadStreamProc, _tiffWriteStreamProc,
        _tiffSeekStreamProc, _tiffCloseStreamProc, _tiffSizeStreamProc,
        _tiffMapProc, _tiffUnmapProc);
    if (tif)
        tif->tif_fd = (int) stream;
    return (tif);
}


/*
 * Open a TIFF file for read/writing.
 */
TIFF*
TIFFOpen(const char* name, const char* mode)
{
    static const char module[] = "TIFFOpen";
    int m, fd;

    m = _TIFFgetMode(mode, module);
    if (m == -1)
        return ((TIFF*)0);

    fd = (int)RwFopen(name, mode);

    if (fd == 0) {
        TIFFError(module, "%s: Cannot open", name);
        return ((TIFF *)0);
    }
    return (TIFFFdOpen(fd, name, mode));
}

TIFF*
TIFFOpenStream(RwStream* stream, const char* name, const char* mode)
{
    static const char module[] = "TIFFOpenStream";
    int m;

    m = _TIFFgetMode(mode, module);
    if (m == -1)
        return ((TIFF*)0);

	/*
    fd = (int)RwFopen(name, mode);

    if (fd == 0) {
        TIFFError(module, "%s: Cannot open", name);
        return ((TIFF *)0);
    }
	*/
    return (TIFFStreamOpen(stream, name, mode));
}


tdata_t
_TIFFmalloc(tsize_t s)
{
    return ((tdata_t)RwMalloc(s,
        rwID_TIFFIMAGEPLUGIN | rwMEMHINTDUR_EVENT));
}

void
_TIFFfree(tdata_t p)
{
    RwFree(p);
    return;
}

tdata_t
_TIFFrealloc(tdata_t p, tsize_t s)
{
    void* pvTmp;

    pvTmp = RwRealloc(p, s, rwID_TIFFIMAGEPLUGIN |
        rwMEMHINTDUR_EVENT | rwMEMHINTFLAG_RESIZABLE);

    return ((tdata_t)pvTmp);
}

void
_TIFFmemset(void* p, int v, tsize_t c)
{
    tsize_t count;

    for (count = 0; count < c; count++)
    {
        ((unsigned char *)p)[count] = (unsigned char)v;
    }
}

void
_TIFFmemcpy(void* d, const tdata_t s, tsize_t c)
{
    tsize_t count;

    for (count = 0; count < c; count++)
    {
        ((unsigned char *)d)[count] = ((unsigned char *)s)[count];
    }
}

int
_TIFFmemcmp(const tdata_t p1, const tdata_t p2, tsize_t c)
{
    const RwUInt8 *pb1 = (const RwUInt8 *) p1;
    const RwUInt8 *pb2 = (const RwUInt8 *) p2;
    RwUInt32 dwTmp = c;
    int iTmp;
    for (iTmp = 0; dwTmp-- && !iTmp; iTmp = (int)*pb1++ - (int)*pb2++)
        ;
    return (iTmp);
}

static void
renderwareWarningHandler(const char* module __RWUNUSED__,
                         const char* fmt __RWUNUSED__,
                         va_list ap __RWUNUSED__)
{
#if 0
    if (module != NULL)
        fprintf(stderr, "%s: ", module);
    fprintf(stderr, "Warning, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
#endif
}
TIFFErrorHandler _TIFFwarningHandler = renderwareWarningHandler;

static void
renderwareErrorHandler(const char* module __RWUNUSED__,
                       const char* fmt __RWUNUSED__,
                       va_list ap __RWUNUSED__)
{
#if 0
    if (module != NULL)
        fprintf(stderr, "%s: ", module);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
#endif
}
TIFFErrorHandler _TIFFerrorHandler = renderwareErrorHandler;
