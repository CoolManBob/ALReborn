// ConfigManager.h: interface for the CConfigManager class.
//
// 일반적인 Config 파일을 읽는 범용 클래스 입니다.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGMANAGER_H__16B54A44_C42A_48A1_B068_B3EC2825A6FE__INCLUDED_)
#define AFX_CONFIGMANAGER_H__16B54A44_C42A_48A1_B068_B3EC2825A6FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CONFIGMANAGER_BRIDGE	'='

class CConfigManager  
{
protected:
	CStdioFile m_csFile;
	BOOL m_bOpen;
	
	CString m_szFileName;
	CString m_szNowLine;
	
public:
	CConfigManager();
	virtual ~CConfigManager();

	void Init();
	void SetFileName(CString& szFileName);

	BOOL Open();
	BOOL Close();

	BOOL ReadLine();
	CString GetKey();
	CString GetValue();
};

#endif // !defined(AFX_CONFIGMANAGER_H__16B54A44_C42A_48A1_B068_B3EC2825A6FE__INCLUDED_)
