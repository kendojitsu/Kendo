// ShortCutDialog.cpp : Implementation files
//

#include "stdafx.h"
#include "kendoUI.h"
#include "ShortCutDialog.h"
#include "afxdialogex.h"


// CShortCutDialog dialog box

IMPLEMENT_DYNAMIC(CShortCutDialog, CDialog)

CShortCutDialog::CShortCutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SHORTCUT_DIALOG, pParent)
{

}

CShortCutDialog::~CShortCutDialog()
{
}

void CShortCutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listCtrlShortCut);
}

void CShortCutDialog::initialListCtrl()
{
	DWORD dwStyle = m_listCtrlShortCut.GetExtendedStyle();	// Add grid lines to list control
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_listCtrlShortCut.SetExtendedStyle(dwStyle);
	m_listCtrlShortCut.GetHeaderCtrl()->EnableWindow(false);// Disable column stretching

	CRect rect;
	m_listCtrlShortCut.GetWindowRect(&rect);
	ScreenToClient(&rect);

	/* Add header */
	int index = 0;
	m_listCtrlShortCut.InsertColumn(++index, "shortcut key", LVCFMT_CENTER, rect.Width() * 0.5);
	m_listCtrlShortCut.InsertColumn(++index, "Function", LVCFMT_CENTER, rect.Width() * 0.5);

	UINT mask = LVIF_PARAM | LVIF_TEXT;
	int row = 0;
	int col = 0;
		
	/* add a row */	
	row = m_listCtrlShortCut.InsertItem(mask, m_listCtrlShortCut.GetItemCount(), "Ctrl + G", 0, 0, 0, NULL);
	m_listCtrlShortCut.SetItemText(row, ++col, "Get the focus of the selected item in the packet list");

	row = m_listCtrlShortCut.InsertItem(mask, m_listCtrlShortCut.GetItemCount(), "Ctrl + O", 0, 0, 0, NULL);
	m_listCtrlShortCut.SetItemText(row, col, "open a file");

	row = m_listCtrlShortCut.InsertItem(mask, m_listCtrlShortCut.GetItemCount(), "Ctrl + W", 0, 0, 0, NULL);
	m_listCtrlShortCut.SetItemText(row, col, "close file");

	row = m_listCtrlShortCut.InsertItem(mask, m_listCtrlShortCut.GetItemCount(), "Ctrl + S", 0, 0, 0, NULL);
	m_listCtrlShortCut.SetItemText(row, col, "Save as");

	row = m_listCtrlShortCut.InsertItem(mask, m_listCtrlShortCut.GetItemCount(), "Alt + F4", 0, 0, 0, NULL);
	m_listCtrlShortCut.SetItemText(row, col, "quit");
}


BEGIN_MESSAGE_MAP(CShortCutDialog, CDialog)
END_MESSAGE_MAP()


// CShortCutDialog message handler


BOOL CShortCutDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add additional initialization here
	initialListCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Exception: OCX property page should return FALSE
}
