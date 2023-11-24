// kendoUI.h : main header file for the kendoUI application
//

#if !defined(AFX_kendoUI_H__F6F0F8D9_180D_4884_AE2B_3F3EF81EC4A8__INCLUDED_)
#define AFX_kendoUI_H__F6F0F8D9_180D_4884_AE2B_3F3EF81EC4A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CkendoUIApp:
// See kendoUI.cpp for the implementation of this class
//

class CkendoUIApp : public CWinApp
{
public:
	CkendoUIApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CkendoUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CkendoUIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_kendoUI_H__F6F0F8D9_180D_4884_AE2B_3F3EF81EC4A8__INCLUDED_)
