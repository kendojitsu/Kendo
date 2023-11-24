#pragma once
#include "afxcmn.h"


// CShortCutDialog dialog box

class CShortCutDialog : public CDialog
{
	DECLARE_DYNAMIC(CShortCutDialog)

public:
	CShortCutDialog(CWnd* pParent = NULL);   // Standard constructor
	virtual ~CShortCutDialog();

// Dialog data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHORTCUT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCtrlShortCut;
	void initialListCtrl();
	virtual BOOL OnInitDialog();
};
