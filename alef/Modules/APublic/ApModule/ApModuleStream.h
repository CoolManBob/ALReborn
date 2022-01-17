// ApModuleStream.h: interface for the ApModuleStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMODULESTREAM_H__167EB431_0B9D_4585_8F52_358FDB0879C6__INCLUDED_)
#define AFX_APMODULESTREAM_H__167EB431_0B9D_4585_8F52_358FDB0879C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
#include "AuIniManager.h"

typedef enum
{
	APMODULE_STREAM_MODE_NORMAL = AUINIMANAGER_MODE_NORMAL,
	APMODULE_STREAM_MODE_NAME_OVERWRITE = AUINIMANAGER_MODE_NAME_OVERWRITE,
	APMODULE_STREAM_MAX_MODE
} ApModuleStreamMode;

typedef enum
{
	APMODULE_STREAM_TYPE_NORMAL		= AUINIMANAGER_TYPE_NORMAL		,
	APMODULE_STREAM_TYPE_PART_INDEX = AUINIMANAGER_TYPE_PART_INDEX	,
	APMODULE_STREAM_TYPE_KEYINDEX	= AUINIMANAGER_TYPE_KEYINDEX	,
	APMODULE_STREAM_MAX_TYPE
} ApModuleStreamType;

class ApModuleStream
{
private:
	AuIniManagerA	m_csIniFile;

protected:
	INT32		m_lSectionID;
	INT32		m_lValueID;
	INT32		m_lModuleDataIndex;

	CHAR		m_szSection[64];
	const CHAR *m_szValueName;
	CHAR *		m_szValue;

	const CHAR *m_szModuleName;
	const CHAR *m_szEnumEnd;

	//@{ 2006/12/08 burumal
	INT32		m_lModuleNameStrLen;
	INT32		m_lEnumEndStrLen;
	//@}

public:
	virtual BOOL Write(const CHAR *szFile, INT32 lPartIndex = 0, BOOL bEncryption = FALSE);
	virtual BOOL SetSection(CHAR *szSection);
	virtual CHAR* GetSectionName();

	virtual BOOL WriteValue(const CHAR *szValueName, FLOAT fValue);
	virtual BOOL WriteValue(const CHAR *szValueName, INT32 lValue);
	virtual BOOL WriteValue(const CHAR *szValueName, INT64 llValue);
	virtual BOOL WriteValue(const CHAR *szValueName, const CHAR *szValue);
	virtual BOOL WriteValue(const CHAR *szValueName, AuPOS *pstPos);

	virtual BOOL ReadNextValue();
	virtual BOOL ReadPrevValue();

	virtual const CHAR *GetValue();
	virtual BOOL GetValue(FLOAT *pfValue);
	virtual BOOL GetValue(INT32 *plValue);
	virtual BOOL GetValue(INT64 *pllValue);
	virtual BOOL GetValue(CHAR *szValue, INT16 nLength);
	virtual BOOL GetValue(AuPOS *pstPos);

	virtual const CHAR * GetValueName();
	virtual BOOL EnumWriteCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule);
	virtual BOOL EnumReadCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule);
	virtual const CHAR * ReadSectionName(INT32 lSectionID);
	virtual INT32 GetNumSections();
	virtual BOOL Open(const CHAR *szFile, INT32 lPartIndex = 0, BOOL bDecryption = FALSE);
	virtual BOOL Parse(char* pBuffer, unsigned long nBufferSize, BOOL bDecryption = FALSE);

	BOOL SetMode(ApModuleStreamMode eMode);
	BOOL SetType(DWORD eType);

	ApModuleStream();
	virtual ~ApModuleStream();

	// Bob님 작업(Add, 051503)-ModelTool에서만 사용...
	VOID SetValueID(INT32 lValueID)	{m_lValueID = lValueID;}

};

#endif // !defined(AFX_APMODULESTREAM_H__167EB431_0B9D_4585_8F52_358FDB0879C6__INCLUDED_)
