// kendoUIDlg.h : header file
//

#include "afxwin.h"
#include "PacketDumper.h"
#include "PacketCatcher.h"
#include "PacketPool.h"
#if !defined(AFX_kendoUIDLG_H__22E6FA67_26EB_4787_8108_560D03B16680__INCLUDED_)
#define AFX_kendoUIDLG_H__22E6FA67_26EB_4787_8108_560D03B16680__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* constant */
const int BITMAP_LIST_MAIN_SIZE = 5;
const int BITMAP_LIST_FILTER_SIZE = 3;
const int BITMAP_WIDTH = 24;
const int BITMAP_HEIGHT = 24;

/////////////////////////////////////////////////////////////////////////////
// CkendoUIDlg dialog

class CkendoUIDlg : public CDialog
{
// Construction
public:
	CkendoUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CkendoUIDlg)
	enum { IDD = IDD_kendoUI_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CkendoUIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	/* control */
	//CRichEditCtrl	richEditCtrlFilterInput_;
	HICON		m_hIcon;
	HACCEL		m_hAccelMenu;
	HACCEL		m_hAccel;
	CMenu		m_menu;
	CToolBar	m_toolBarMain;
	CToolBar	m_toolBarFilter;
	CFont       m_comboFont;		//��ʾcombobox������
	CComboBox	m_comboBoxDevList;
	CComboBox	m_comboBoxFilterList;
	CListCtrl	m_listCtrlPacketList;
	CTreeCtrl	m_treeCtrlPacketDetails;
	CEdit		m_editCtrlPacketBytes;
	CStatusBar  m_statusBar;

	/*logo */
	CBitmap		m_bitmapListMain[BITMAP_LIST_MAIN_SIZE];
	CBitmap		m_bitmapListFilter[BITMAP_LIST_FILTER_SIZE];
	CImageList	m_imageListMain;
	CImageList	m_imageListFilter;

	/* logo */
	bool    m_pktCaptureFlag;
	bool	m_fileOpenFlag;
	CString m_openFileName;	// Save the file name of the open file

	/* Data packet related classes */
	PacketCatcher	m_catcher;
	PacketPool		m_pool;

	/* File related classes */
	PacketDumper	m_pktDumper;

	/* ��ʼ����غ��� */
	void initialAccelerator();
	void initialMenuBar();
	void initialToolBar();
	void initialComboBoxDevList();
	void initialComboBoxFilterList();
	void initialListCtrlPacketList();
	void initialTreeCtrlPacketDetails();
	void initialEditCtrlPacketBytes();
	void initialStatusBar();

	void updateStatusBar(const CString &status, int pktTotalNum, int pktDisplayNum);
	
	/* Folder operation related functions */
	bool createDirectory(const CString& dirPath);
	bool clearDirectory(const CString& dirPath);
	
	/* Activate shortcut keys and add calls to shortcut key handling */
	virtual BOOL  PreTranslateMessage(MSG*  pMsg);

	// Generated message map functions
	//{{AFX_MSG(CkendoUIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClickedStart();
	afx_msg void OnClickedStop();
	afx_msg void OnClickedFilter();
	afx_msg void OnClickedClear();
	afx_msg void OnClickedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnPktCatchMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTExitMessage(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);  // ��������ʾ  
	afx_msg void OnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuFileOpen();
	afx_msg void OnMenuFileClose();
	afx_msg void OnMenuFileSaveAs();
	afx_msg void OnMenuFileClearCache();
	afx_msg void OnMenuFileExit();
	afx_msg void OnMenuHelpShortCut();
	afx_msg void OnMenuHelpAbout();
	//afx_msg void OnUpdateStatus(CCmdUI *pCmdUI);
	afx_msg void OnAcceleratorCtrlG();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	/* Control printing related functions */
	int printListCtrlPacketList(const Packet &pkt);
	int printListCtrlPacketList(PacketPool &pool);
	int printListCtrlPacketList(PacketPool &pool, const CString &filter);

	int	printEditCtrlPacketBytes(const Packet &pkt);

	int printTreeCtrlPacketDetails(const Packet &pkt);
	int printEthernet2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printIP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printARP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printICMP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printTCP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printUDP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printDNS2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printDHCP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
	int	printHTTP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);

	int printDNSHeader(const Packet &pkt, HTREEITEM & parentNode);
	HTREEITEM printDNSBanner(const Packet &pkt, HTREEITEM &parentNode);
	CString DNSType2CString(const u_short &type);
	CString DNSClass2CString(const u_short &classes);
	int printDNSQuery(char *DNSQuery, const u_short &questions, HTREEITEM &parentNode);
	int printDNSResourceRecord(char *DNSResourceRecord, const u_short &resourceRecordNum, const int &resourceRecordType, const DNS_Header *pDNSHeader, HTREEITEM parentNode);

	/* Convert */
	CString	MACAddr2CString(const MAC_Address &addr);
	CString	IPAddr2CString(const IP_Address &addr);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_kendoUIDLG_H__22E6FA67_26EB_4787_8108_560D03B16680__INCLUDED_)
