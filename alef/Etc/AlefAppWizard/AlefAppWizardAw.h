#if !defined(AFX_ALEFAPPWIZARDAW_H__FFE36F07_4363_40DC_A316_9A44C3C3287E__INCLUDED_)
#define AFX_ALEFAPPWIZARDAW_H__FFE36F07_4363_40DC_A316_9A44C3C3287E__INCLUDED_

// AlefAppWizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see AlefAppWizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CAlefAppWizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);
};

// This declares the one instance of the CAlefAppWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global AlefAppWizardaw.  (Its definition is in AlefAppWizardaw.cpp.)
extern CAlefAppWizardAppWiz AlefAppWizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALEFAPPWIZARDAW_H__FFE36F07_4363_40DC_A316_9A44C3C3287E__INCLUDED_)
