/*=====================================================================

	AgsModuleDBStream.h
		
=====================================================================*/

#ifndef _AGSMODULE_DB_STREAM_H_
	#define _AGSMODULE_DB_STREAM_H_

#include "ApDefine.h"
#include "ApModule.h"
#include "ApModuleStream.h"
#include "AuDatabase.h"

class ApModule;

class AgsModuleDBStream : public ApModuleStream
	{
	private:
		// ... db
		AuDatabaseManager	m_csManager;
		AuDatabase			*m_pDatabase;
		AuStatement			*m_pStatement;
		AuRowset			*m_pRowset;

		// ... position of "Section" Column
		INT16		m_nSectionColumnIndex;
	
	private:
		void		_Close();

	public:
		AgsModuleDBStream();
		virtual ~AgsModuleDBStream();

		// Init
		BOOL		Initialize(eAUDB_VENDER eVender = AUDB_VENDER_ORACLE);
		
		// open
		BOOL		Open(CHAR *pszDatabase, CHAR *pszUser, CHAR *pszPass, CHAR *pszTable, INT32 lPartIndex = 0);

		// write method
		BOOL		Write(const CHAR *pszTable, INT32 lPartIndex = 0);	//???

		BOOL		SetSection(CHAR *pszSection);
		BOOL		WriteValue(CHAR *pszName, FLOAT fValue);
		BOOL		WriteValue(CHAR *pszName, INT32 lValue);
		BOOL		WriteValue(CHAR *pszName, INT64 llValue);
		BOOL		WriteValue(CHAR *pszName, CHAR *pszValue);
		BOOL		WriteValue(CHAR *pszName, AuPOS *pstPos);

		// read method
		BOOL		ReadNextValue();
		BOOL		ReadPrevValue();
		const CHAR*	ReadSectionName(INT32 lSectionID);

		const CHAR*	GetValueName();
		const CHAR*	GetValue();
		BOOL		GetValue(FLOAT *pfValue);
		BOOL		GetValue(INT32 *plValue);
		BOOL		GetValue(INT64 *pllValue);
		BOOL		GetValue(CHAR *pszValue, INT16 nLength);
		BOOL		GetValue(AuPOS *pstPos);

		// enum callback
		BOOL EnumWriteCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule);
		BOOL EnumReadCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule);

		// Info
		INT32 GetNumSections();
		BOOL SetMode(ApModuleStreamMode eMode);
		BOOL SetType(ApModuleStreamType eType);
		
		
		// Custom
		BOOL		OpenCustom(CHAR *pszDSN, CHAR *pszUser, CHAR *pszPassword);
		BOOL		QueryCustom(CHAR *pszQuery);
		CHAR*		GetCustomValue(UINT32 r, UINT32 c);
		
	};

#endif
