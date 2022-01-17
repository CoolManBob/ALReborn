#include "stdafx.h"
#include "EffUt_Vtx.h"

EFFUT_BEGIN

const DWORD VTX_PT1::FVF = D3DFVF_XYZ  | D3DFVF_TEX1;
const DWORD VTX_PT1::SIZE = sizeof( VTX_PT1 );
VTX_PT1::VTX_PT1() : 
 pos( 0.f, 0.f, 0.f ),
 uv( 0.f, 0.f)
{
};

VTX_PT1::VTX_PT1(const VTX_PT1& cpy) :
 pos( cpy.pos ),
 uv( cpy.uv )
{
};

VTX_PT1& VTX_PT1::operator = (const VTX_PT1& cpy)
{
	pos	= cpy.pos;
	uv	= cpy.uv;

	return *this;
};

const DWORD VTX_PD::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
const DWORD VTX_PD::SIZE = sizeof(VTX_PD);
VTX_PD::VTX_PD() :
 pos( 0.f, 0.f, 0.f ),
 diff( 0xffffffff )
{
};

VTX_PD::VTX_PD(const VTX_PD& cpy) :
 pos( cpy.pos ),
 diff( cpy.diff )
{
};

VTX_PD& VTX_PD::operator = (const VTX_PD& cpy)
{
	pos		= cpy.pos;
	diff	= cpy.diff;

	return *this;
};

const DWORD VTX_PNDT1::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const DWORD VTX_PNDT1::SIZE	= sizeof(VTX_PNDT1);
VTX_PNDT1::VTX_PNDT1() :
 pos( 0.f, 0.f, 0.f ),
 nrm( 0.f, 0.f, 0.f ),
 diff( 0xffffffff ),
 uv( 0.f, 0.f )
{
};

VTX_PNDT1::VTX_PNDT1(const VTX_PNDT1& cpy) :
 pos(cpy.pos),
 nrm(cpy.nrm),
 diff(cpy.diff),
 uv(cpy.uv)
{
};
VTX_PNDT1& VTX_PNDT1::operator = (const VTX_PNDT1& cpy)
{
	pos		= cpy.pos;
	nrm		= cpy.nrm;
	diff	= cpy.diff;
	uv		= cpy.uv;

	return *this;
};

const DWORD VTX_PDT1::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const DWORD VTX_PDT1::SIZE = sizeof(VTX_PDT1);
VTX_PDT1::VTX_PDT1() :
 pos( 0.f, 0.f, 0.f ),
 diff( 0xffffffff ),
 uv( 0.f, 0.f )
{
};

VTX_PDT1::VTX_PDT1(const VTX_PDT1& cpy) :
 pos( cpy.pos ),
 diff( cpy.diff ),
 uv( cpy.uv )
{
};

VTX_PDT1& VTX_PDT1::operator = (const VTX_PDT1& cpy)
{
	pos		= cpy.pos;
	diff	= cpy.diff;
	uv		= cpy.uv;

	return *this;
};

const DWORD VTX_PDST1::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;
const DWORD VTX_PDST1::SIZE = sizeof(VTX_PDST1);
VTX_PDST1::VTX_PDST1() :
 pos( 0.f, 0.f, 0.f ),
 diff( 0xffffffff ),
 spec( 0xffffffff ),
 uv( 0.f, 0.f )
{
};

VTX_PDST1::VTX_PDST1(const VTX_PDST1& cpy) :
 pos( cpy.pos ),
 diff( cpy.diff ),
 spec( cpy.spec ),
 uv( cpy.uv )
{
};

VTX_PDST1& VTX_PDST1::operator =(const VTX_PDST1& cpy)
{
	pos		= cpy.pos;
	diff	= cpy.diff;
	spec	= cpy.spec;
	uv		= cpy.uv;

	return *this;
};

const DWORD VTX_PDS::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
const DWORD VTX_PDS::SIZE = sizeof(VTX_PDS);
VTX_PDS::VTX_PDS()
: pos(0.f,0.f,0.f)
, diff(0xffffffff)
, spec(0xffffffff)
{
};

VTX_PDS::VTX_PDS(const VTX_PDS& cpy)
: pos(cpy.pos)
, diff(cpy.diff)
, spec(cpy.spec)
{
};

VTX_PDS& VTX_PDS::operator = (const VTX_PDS& cpy)
{
	pos		= cpy.pos;
	diff	= cpy.diff;
	spec	= cpy.spec;

	return *this;
};

const DWORD VTX_RHWT1::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
const DWORD VTX_RHWT1::SIZE = sizeof(VTX_RHWT1);
VTX_RHWT1::VTX_RHWT1() :
 rhw( 0.f, 0.f,  0.f, 1.f ),
 uv( 0.f, 0.f )
{
};

VTX_RHWT1::VTX_RHWT1(const VTX_RHWT1& cpy) :
 rhw( cpy.rhw ),
 uv ( cpy.uv )
{
};

VTX_RHWT1& VTX_RHWT1::operator =(const VTX_RHWT1& cpy)
{
	rhw = cpy.rhw;
	uv = cpy.uv;

	return *this;
};

const DWORD VTX_RHWD::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
const DWORD VTX_RHWD::SIZE = sizeof(VTX_RHWD);
VTX_RHWD::VTX_RHWD() :
 rhw( 0.f, 0.f, 0.f, 1.f ),
 diff(0xffffffff)
{
};

VTX_RHWD::VTX_RHWD(const VTX_RHWD& cpy) :
 rhw( cpy.rhw ),
 diff ( cpy.diff )
{
};

VTX_RHWD& VTX_RHWD::operator =(const VTX_RHWD& cpy)
{
	rhw = cpy.rhw;
	diff = cpy.diff;

	return *this;
};

EFFUT_END