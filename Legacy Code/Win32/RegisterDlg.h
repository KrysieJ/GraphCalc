#if !defined(AFX_REGISTERDLG_H__195C97A6_3C59_49A6_87C8_4C225D996710__INCLUDED_)
#define AFX_REGISTERDLG_H__195C97A6_3C59_49A6_87C8_4C225D996710__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
	// Construction
public:
	CRegisterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_REGISTER };
	CString	m_sName;
	CString	m_sCode;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDLG_H__195C97A6_3C59_49A6_87C8_4C225D996710__INCLUDED_)
