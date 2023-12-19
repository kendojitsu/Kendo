// kendoUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kendoUI.h"
#include "kendoUIDlg.h"
#include "ThreadParam.h"
#include "Global.h"
#include "PacketCatcher.h"
#include <vector>
#include "ShortCutDialog.h"
#define HAVE_REMOTE
#include "pcap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "version.lib")	// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	BOOL OnInitDialog();
	//void OnShowWindow(BOOL bShow, UINT nStatus);
	//CString CAboutDlg::GetAppVersion(CString *AppName);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	/*Get program path*/
	WCHAR l_wcaAppPath[MAX_PATH];//Save application path
	::GetModuleFileName(NULL, (LPSTR)l_wcaAppPath, MAX_PATH);

	/* Get version information size */
	UINT l_uiVersionInfoSize;//The overall size of the saved version information
	TCHAR * l_ptcVersionInfo;
	l_uiVersionInfoSize = ::GetFileVersionInfoSize((LPSTR)l_wcaAppPath, 0);//get size
	l_ptcVersionInfo = new TCHAR[l_uiVersionInfoSize];//Apply for space 

	 /* This structure is used to obtain language information of version information */
	struct VersionLanguage
	{
		WORD m_wLanguage;
		WORD m_wCcodePage;
	};

	VersionLanguage * l_ptVersionLanguage;
	UINT l_uiSize;

	if (::GetFileVersionInfo((LPSTR)l_wcaAppPath, 0, l_uiVersionInfoSize, l_ptcVersionInfo) != 0)//Get version information
	{

		if (::VerQueryValue(l_ptcVersionInfo, _T("\\VarFileInfo\\Translation"), reinterpret_cast<LPVOID*>(&l_ptVersionLanguage), &l_uiSize))//Query language information and save
		{
			/* Generate query information formatter */
			CString l_cstrSubBlock;
			l_cstrSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\ProductVersion"), l_ptVersionLanguage->m_wLanguage, l_ptVersionLanguage->m_wCcodePage);

			LPVOID * l_pvResult;

			/* Query specified information */
			if (::VerQueryValue(static_cast<LPVOID>(l_ptcVersionInfo), l_cstrSubBlock.GetBuffer(), reinterpret_cast<LPVOID*>(&l_pvResult), &l_uiSize))
			{
				CString l_cstrProductVersion(reinterpret_cast<TCHAR *>(l_pvResult));// Get version information
				GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextA("ver "+ l_cstrProductVersion);// Version information is printed to the About window
			}

		}
	}

	delete[] l_ptcVersionInfo;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CkendoUIDlg dialog

CkendoUIDlg::CkendoUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CkendoUIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CkendoUIDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_catcher.setPool(&m_pool);		// catcher initialization
	//m_dumper.setPool(&m_pool);		// dumper initialization

	/* Flag initialization */
	m_pktCaptureFlag = false;
	m_fileOpenFlag = false;	
}

void CkendoUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CkendoUIDlg)
	DDX_Control(pDX, IDC_LIST1, m_listCtrlPacketList);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrlPacketDetails);
	DDX_Control(pDX, IDC_EDIT1, m_editCtrlPacketBytes);
	//DDX_Control(pDX, IDC_RICHEDIT21, richEditCtrlFilterList_);
	//}}AFX_DATA_MAP

	
}

BEGIN_MESSAGE_MAP(CkendoUIDlg, CDialog)
	//{{AFX_MSG_MAP(CkendoUIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickedList1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CkendoUIDlg::OnCustomdrawList1)
	ON_MESSAGE(WM_PKTCATCH, &CkendoUIDlg::OnPktCatchMessage)
	ON_MESSAGE(WM_TEXIT, &CkendoUIDlg::OnTExitMessage)
	//}}AFX_MSG_MAP

	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, &CkendoUIDlg::OnKeydownList1)
	ON_COMMAND(ID_MENU_FILE_OPEN, &CkendoUIDlg::OnMenuFileOpen)
	ON_COMMAND(ID_MENU_FILE_CLOSE, &CkendoUIDlg::OnMenuFileClose)
	ON_COMMAND(ID_MENU_FILE_CLEAR_CACHE, &CkendoUIDlg::OnMenuFileClearCache)
	ON_COMMAND(ID_MENU_FILE_SAVEAS, &CkendoUIDlg::OnMenuFileSaveAs)
	ON_COMMAND(ID_MENU_FILE_EXIT, &CkendoUIDlg::OnMenuFileExit)
	ON_COMMAND(ID_MENU_HELP_ABOUT, &CkendoUIDlg::OnMenuHelpAbout)
	ON_COMMAND(ID_MENU_HELP_SHORTCUT, &CkendoUIDlg::OnMenuHelpShortCut)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_STATUS, &CkendoUIDlg::OnUpdateStatus)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xffff, OnToolTipText)
	ON_COMMAND(ID_TOOLBARBTN_START, &CkendoUIDlg::OnClickedStart)
	ON_COMMAND(ID_TOOLBARBTN_STOP, &CkendoUIDlg::OnClickedStop)
	ON_COMMAND(ID_TOOLBARBTN_CLEAR, &CkendoUIDlg::OnClickedClear)
	ON_COMMAND(ID_TOOLBARBTN_FILTER, &CkendoUIDlg::OnClickedFilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CkendoUIDlg message handlers

/**
*	@brief UI interface initialization
*	@param	-
*	@return	-
*/
BOOL CkendoUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	initialAccelerator();				// Shortcut key initialization
	initialMenuBar();					// Menu bar initialization
	initialToolBar();					// Toolbar initialization
	initialComboBoxDevList();			// Network card list initialization
	initialComboBoxFilterList();		// Filter list initialization
	initialListCtrlPacketList();		// List control (packet list) initialization
	initialTreeCtrlPacketDetails();		// Tree control (packet details) initialization
	initialEditCtrlPacketBytes();		// Edit control (packet byte stream) initialization
	initialStatusBar();					// Status bar initialization¯
	createDirectory(".\\tmp");			// Determine whether the tmp folder exists, create it if it does not exist
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CkendoUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CkendoUIDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CkendoUIDlg::OnQueryDragIcon()
{

	return (HCURSOR) m_hIcon;
}
/*************************************************************
*
*		Button event implementation
*
*************************************************************/
/**
*	@brief	Press the start button to start capturing packets
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnClickedStart()
{
	// Get current time
	time_t tt = time(NULL);	// This sentence returns only a timestamp
	localtime(&tt);
	CTime currentTime(tt);

	/* If the network card is not selected, a prompt message will be reported; otherwise, a thread will be created to capture packets. */
	int selItemIndex = m_comboBoxDevList.GetCurSel();
	if (selItemIndex <= 0)
	{
		AfxMessageBox(_T("Please select network card"), MB_OK);
		return;
	}

	if (m_catcher.openAdapter(selItemIndex, currentTime))
	{
		CString status = "capturing:" + m_catcher.getDevName();
		/* Modify control enable status */
		m_comboBoxDevList.EnableWindow(FALSE);
		m_toolBarMain.GetToolBarCtrl().EnableButton(ID_TOOLBARBTN_START, FALSE);
		m_toolBarMain.GetToolBarCtrl().EnableButton(ID_TOOLBARBTN_STOP, TRUE);

		m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_OPEN, FALSE);
		m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_SAVEAS, FALSE);

		m_menu.EnableMenuItem(ID_MENU_FILE_OPEN, MF_GRAYED);	// Disable menu item "Open"
		m_menu.EnableMenuItem(ID_MENU_FILE_CLOSE, MF_GRAYED);	// Disable menu item "Close"
		m_menu.EnableMenuItem(ID_MENU_FILE_OPEN, MF_GRAYED);	// Disable menu item "Save As"
		
		/* Çå¿Õ¿Ø¼þÏÔÊ¾ÄÚÈÝ */
		m_listCtrlPacketList.DeleteAllItems();
		m_treeCtrlPacketDetails.DeleteAllItems();
		m_editCtrlPacketBytes.SetWindowTextA("");

		AfxGetMainWnd()->SetWindowText(status);

		/* Clear the in-memory packet pool */
		m_pool.clear();

		/* Update status bar */
		updateStatusBar(status, m_pool.getSize(), m_listCtrlPacketList.GetItemCount());

		CString fileName = "kendoUI_" + currentTime.Format("%Y%m%d%H%M%S") + ".pcap";
		m_pktDumper.setPath(".\\tmp\\" + fileName);

		m_catcher.startCapture(MODE_CAPTURE_LIVE);
		m_pktCaptureFlag = true;

		m_openFileName = fileName;
		m_fileOpenFlag = true;
	}
}

/**
*	@brief	Press the end button to stop packet capture, delete the printed packet-related information, clear the packet list, and restart packet capture.
*	@param	-
*	@return -
*/
void CkendoUIDlg::OnClickedStop() 
{
	CString status = "Capture ends:" + m_catcher.getDevName();
	AfxGetMainWnd()->SetWindowText(m_pktDumper.getPath());	// Modify title bar

	m_comboBoxDevList.EnableWindow(TRUE);
	m_toolBarMain.GetToolBarCtrl().EnableButton(ID_TOOLBARBTN_START, TRUE);
	m_toolBarMain.GetToolBarCtrl().EnableButton(ID_TOOLBARBTN_STOP, FALSE);
	m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_OPEN, TRUE);
	m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_SAVEAS, TRUE);

	m_menu.EnableMenuItem(ID_MENU_FILE_OPEN, MF_ENABLED);	// Enable menu item "Open"
	m_menu.EnableMenuItem(ID_MENU_FILE_CLOSE, MF_GRAYED);	// Disable menu item "Close"
	m_menu.EnableMenuItem(ID_MENU_FILE_SAVEAS, MF_ENABLED);	// Enable menu item "Save as"
	m_statusBar.SetPaneText(0, status, true);			// Modify status

	m_catcher.stopCapture();
	m_pktCaptureFlag = false;
	//m_catcher.closeAdapter();
}

/**
*	@brief	Press the filter button to filter packets based on the protocol name entered in the filter
*	@param	-
*	@return -
*/
void CkendoUIDlg::OnClickedFilter()
{
	int selIndex = m_comboBoxFilterList.GetCurSel();
	if (selIndex <= 0)
		return;
	CString strFilter;
	m_comboBoxFilterList.GetLBText(selIndex, strFilter);

	m_listCtrlPacketList.DeleteAllItems();
	m_treeCtrlPacketDetails.DeleteAllItems();
	m_editCtrlPacketBytes.SetWindowTextA("");

	printListCtrlPacketList(m_pool, strFilter);
	updateStatusBar(CString(""), m_pool.getSize(), m_listCtrlPacketList.GetItemCount());
}

/**
*	@brief	Press the clear button to clear the filter and display all packets
*	@param	-
*	@return -
*/
void CkendoUIDlg::OnClickedClear()
{
	m_comboBoxFilterList.SetCurSel(0);
	m_listCtrlPacketList.DeleteAllItems();
	m_treeCtrlPacketDetails.DeleteAllItems();
	m_editCtrlPacketBytes.SetWindowTextA("");

	printListCtrlPacketList(m_pool);
	updateStatusBar(CString(""), m_pool.getSize(), m_listCtrlPacketList.GetItemCount());
}
/*************************************************************
*
*		Control initialization
*
*************************************************************/
/**
*	@brief	Shortcut key initialization
*	@param	-
*	@return	-
*/
void CkendoUIDlg::initialAccelerator()
{
	m_hAccelMenu = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU1));	// ¼ÓÔØ²Ëµ¥¿ì½Ý¼ü×ÊÔ´
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
}

/**
*	@brief	Menu bar initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialMenuBar()
{
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);

	/* Menu item disabled */
//	CMenu* pMenu = this->GetMenu();
	if (m_menu)
	{
		m_menu.EnableMenuItem(ID_MENU_FILE_CLOSE, MF_GRAYED);	// Disable menu item "Close"
		m_menu.EnableMenuItem(ID_MENU_FILE_SAVEAS, MF_GRAYED);	// Disable menu item "Save As"
	}
}

/**
*	@brief	Toolbar initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialToolBar()
{
	// Main toolbar creation 
	if (!m_toolBarMain.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBarMain.LoadToolBar(IDR_TOOLBAR1))
	{
		AfxMessageBox(_T("Failed to create main tool bar\n"));
		return; 
	}

	// Create combo box (list of network cards) on main toolbar button
	//Create a combo box on the button, the button position determines the position of the combo box
	int index = m_toolBarMain.CommandToIndex(ID_TOOLBARBTN_DEVLIST);
	m_toolBarMain.SetButtonInfo(index, ID_TOOLBARBTN_DEVLIST, TBBS_SEPARATOR, 300);//ÉèÖÃ×éºÏ¿òµÄID£¬ÀàÐÍ£¨ÕâÀïÊÇ·Ö¸ôÀ¸£©£¬300ÊÇÖ¸·Ö¸ôÀ¸¿í¶È

	// Create a combo box based on the size rect of the separator																	  
	CRect rect;
	m_toolBarMain.GetItemRect(index, &rect);
	rect.left += 10;
	rect.top += 3;
	m_comboBoxDevList.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, rect, &m_toolBarMain, ID_TOOLBARBTN_DEVLIST);

	// Read the main toolbar button icon, store it in ImageList, and the toolbar reads ImageList
	m_imageListMain.Create(BITMAP_WIDTH, BITMAP_HEIGHT, ILC_COLOR24 | ILC_MASK, 0, 0);
	for (int i = 0; i < BITMAP_LIST_MAIN_SIZE; ++i)
	{
		m_bitmapListMain[i].LoadBitmapA(IDB_BITMAP_DEV + i);
		m_imageListMain.Add(&m_bitmapListMain[i], RGB(0, 0, 0));
	}
	m_toolBarMain.GetToolBarCtrl().SetImageList(&m_imageListMain);

	// Disable buttons on the main toolbar
	//m_toolBarMain.GetToolBarCtrl().EnableButton(IDC_DROPDOWNBTN_DEVLIST, FALSE);
	m_toolBarMain.GetToolBarCtrl().EnableButton(ID_TOOLBARBTN_STOP, FALSE);
	m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_SAVEAS, FALSE);

	
	// Filter toolbar creation
	if (!m_toolBarFilter.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBarFilter.LoadToolBar(IDR_TOOLBAR2))
	{
		AfxMessageBox(_T("Failed to create filter tool bar\n"));
		return;
	}

	// Create combobox (filter list) on filter toolbar button
    index = m_toolBarFilter.CommandToIndex(ID_TOOLBARBTN_FILTERLIST);
	m_toolBarFilter.SetButtonInfo(index, ID_TOOLBARBTN_FILTERLIST, TBBS_SEPARATOR, 300);//ÉèÖÃ×éºÏ¿òµÄID£¬ÀàÐÍ£¨ÕâÀïÊÇ·Ö¸ôÀ¸£©£¬300ÊÇÖ¸·Ö¸ôÀ¸¿í¶È

	// Create a combo box based on the size rect of the separator
	m_toolBarFilter.GetItemRect(index, &rect);
	rect.left += 10;
	rect.top += 3;
	m_comboBoxFilterList.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, rect, &m_toolBarFilter, ID_TOOLBARBTN_FILTERLIST);

	// Read the filter toolbar button icon, store it in ImageList, and the toolbar reads ImageList
	m_imageListFilter.Create(BITMAP_WIDTH, BITMAP_HEIGHT, ILC_COLOR24 | ILC_MASK, 0, 0);
	for (int i = 0; i < BITMAP_LIST_FILTER_SIZE; ++i)
	{
		m_bitmapListFilter[i].LoadBitmapA(IDB_BITMAP_DEV + BITMAP_LIST_MAIN_SIZE + i);
		m_imageListFilter.Add(&m_bitmapListFilter[i], RGB(0, 0, 0));
	}
	m_toolBarFilter.GetToolBarCtrl().SetImageList(&m_imageListFilter);

	// Set drop-down list font
	m_comboFont.CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "ÐÂËÎÌå");
	m_comboBoxDevList.SetFont(&m_comboFont);
	m_comboBoxFilterList.SetFont(&m_comboFont);

	// Set drop-down list height
	m_comboBoxDevList.SetItemHeight(-1, 18);
	m_comboBoxFilterList.SetItemHeight(-1, 18);

	//Control strip positioning
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

/**
*	@brief	Get the local machine network card list and print the network card description to the drop-down list
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialComboBoxDevList()
{
	m_comboBoxDevList.AddString("Select network card");
	m_comboBoxDevList.SetCurSel(0);

	pcap_if_t *dev = NULL; 
	pcap_if_t *allDevs = NULL;
	char errbuf[PCAP_ERRBUF_SIZE + 1];
	if (pcap_findalldevs(&allDevs, errbuf) == -1)
	{
		AfxMessageBox(_T("pcap_findalldevs wrong!"), MB_OK);
		return;
	}
	for (dev = allDevs; dev != NULL; dev = dev->next)
	{
		if (dev->description != NULL)
			m_comboBoxDevList.AddString(dev->description);		
	}
	m_catcher.setDevList(allDevs);
	//pcap_freealldevs(allDevs);
}

/**
*	@brief	Filter list initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialComboBoxFilterList()
{
	std::vector<CString> filterList;
	filterList.push_back("Ethernet");
	filterList.push_back("IP");
	filterList.push_back("ARP");
	filterList.push_back("ICMP");
	filterList.push_back("TCP");
	filterList.push_back("UDP");
	filterList.push_back("DNS");
	filterList.push_back("DHCP");
	filterList.push_back("HTTP");

	m_comboBoxFilterList.AddString("Select filter(optional)");
	m_comboBoxFilterList.SetCurSel(0);

	for(int i = 0; i < filterList.size(); ++i)
		m_comboBoxFilterList.AddString(filterList[i]);
}

/**
*	@brief	Tree control (packet details) initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialListCtrlPacketList()
{
	// Adjust list control (packet list) position based on filter toolbar position
	CRect rect;
	m_toolBarFilter.GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_LIST1)->SetWindowPos(NULL, rect.left, rect.bottom + 5, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	
	DWORD dwStyle = m_listCtrlPacketList.GetExtendedStyle();	// Ìí¼ÓÁÐ±í¿Ø¼þµÄÍø¸ñÏß
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP;
	m_listCtrlPacketList.SetExtendedStyle(dwStyle);

	m_listCtrlPacketList.GetWindowRect(&rect);
	ScreenToClient(&rect);

	int index = 0;
	m_listCtrlPacketList.InsertColumn(index, "number", LVCFMT_CENTER, rect.Width() * 0.05);
	m_listCtrlPacketList.InsertColumn(++index, "time", LVCFMT_CENTER, rect.Width() * 0.15);
	m_listCtrlPacketList.InsertColumn(++index, "agreement", LVCFMT_CENTER, rect.Width() * 0.05);
	m_listCtrlPacketList.InsertColumn(++index, "length", LVCFMT_CENTER, rect.Width() * 0.05);
	m_listCtrlPacketList.InsertColumn(++index, "MAC address·", LVCFMT_CENTER, rect.Width() * 0.175);
	m_listCtrlPacketList.InsertColumn(++index, "destination MAC address", LVCFMT_CENTER, rect.Width() * 0.175);
	m_listCtrlPacketList.InsertColumn(++index, "Source IP address", LVCFMT_CENTER, rect.Width() * 0.175);
	m_listCtrlPacketList.InsertColumn(++index, "destination IP address", LVCFMT_CENTER, rect.Width() * 0.175);

}

/**
*	@brief	Tree control (packet details) initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialTreeCtrlPacketDetails()
{
	// Adjust the position of the tree control (packet details) based on the position of the list control (packet list)
	CRect rect, winRect;
	m_listCtrlPacketList.GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_TREE1)->SetWindowPos(NULL, rect.left, rect.bottom + 5, rect.Width() * 0.5, rect.Height() + 125, SWP_NOZORDER);
}

/**
*	@brief	Edit control (packet byte stream) initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialEditCtrlPacketBytes()
{
	// Adjust the position of the edit control (packet byte stream) based on the position of the tree control control (packet details)
	CRect rect;
	m_treeCtrlPacketDetails.GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL, rect.right + 5, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

/**
*	@brief	Status bar initialization
*	@param	-
*	@return -
*/
void CkendoUIDlg::initialStatusBar()
{
	if (m_statusBar.Create(this))	// Create menu bar
	{
		static UINT indicators[] =
		{
			ID_INDICATOR_STATUS,
			ID_INDICATOR_PKT_TOTAL_NUM,
			ID_INDICATOR_PKT_DISPLAY_NUM
		};
		int indicatorsSize = sizeof(indicators) / sizeof(UINT);
		m_statusBar.SetIndicators(indicators, indicatorsSize);
		CRect rect;
		GetClientRect(rect);
		int index = 0;
		m_statusBar.SetPaneInfo(index, ID_INDICATOR_STATUS, SBPS_STRETCH, rect.Width() * 0.6);
		m_statusBar.SetPaneInfo(++index, ID_INDICATOR_PKT_TOTAL_NUM, SBPS_NORMAL, rect.Width() * 0.2);
		m_statusBar.SetPaneInfo(++index, ID_INDICATOR_PKT_DISPLAY_NUM, SBPS_NORMAL, rect.Width() * 0.15);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); // Show status bar
	}
}
/**
*	@brief	Update status bar
*	@param [in]	status	state
*	@param [in]	pktTotalNum	Total number of packets. Update this word when the value is non-negative.
*	@param [in]	pktDisplayNum	Display number of data packets. Update this field when the value is non-negative.
*	@return	-
*/
void CkendoUIDlg::updateStatusBar(const CString & status, int pktTotalNum, int pktDisplayNum)
{
	if (!status.IsEmpty())
	{
		int index = m_statusBar.CommandToIndex(ID_INDICATOR_STATUS);
		m_statusBar.SetPaneText(index, status, TRUE);
	}
	if (pktTotalNum >= 0)
	{
		int index = m_statusBar.CommandToIndex(ID_INDICATOR_PKT_TOTAL_NUM);
		CString text;
		text.Format("data package", pktTotalNum);
		m_statusBar.SetPaneText(index, text, TRUE);
	}
	if (pktDisplayNum >= 0)
	{
		int index = m_statusBar.CommandToIndex(ID_INDICATOR_PKT_DISPLAY_NUM);
		CString text;
		double percentage = (pktDisplayNum == 0 || pktTotalNum == 0)? 
			0.0 : ((double)pktDisplayNum / pktTotalNum * 100);
		text.Format("already shown: %d (%.1f%%)", pktDisplayNum, percentage);
		m_statusBar.SetPaneText(index, text, TRUE);
	}
}

/**
*	@brief	Create a folder at the specified path
*	@param [in]	dirPath	 folder path
*	@return	true Creation successful false Creation failed (folder already exists)
*/
bool CkendoUIDlg::createDirectory(const CString& dirPath)
{
	if (!PathIsDirectory(dirPath.GetString()))  // Is there a folder with the same name?
	{
		::CreateDirectory(dirPath.GetString(), 0);
		return true;
	}
	return false;
}

/**
*	@brief	Clear all files in the specified folder
*	@param [in]	dirPath	 folder path
*	@return	true if the clearing is successful false if the clearing fails
*/
bool CkendoUIDlg::clearDirectory(const CString& dirPath)
{
		CFileFind finder;
		CString path(dirPath);
		path += _T("\\*.*");

		BOOL isFound = finder.FindFile(path);
		if (!isFound)
		{
			return false;
		}
		while (isFound)
		{
			isFound = finder.FindNextFile();

			// Skip . and .. ; otherwise you will get stuck in an infinite loop
			if (finder.IsDots())
				continue;

			// If it is a directory, enter the search (recursively)
			if (finder.IsDirectory())
			{
				CString subDirPath = dirPath + finder.GetFileName();
				clearDirectory(subDirPath); // Delete files in a folder
				RemoveDirectory(subDirPath); // Remove empty files
			}
			else
			{
				CString filePath = dirPath + finder.GetFileName();
				DeleteFile(filePath);
			}
		}
		finder.Close();
		return true;
}

/**
*	@brief	Print packet summary information to a list control
*	@param	data pack
*	@return	0 Printing successful -1 Printing failed
*/
int CkendoUIDlg::printListCtrlPacketList(const Packet &pkt)
{
	if (pkt.isEmpty())
		return -1;

	int row = 0;	// Line number
	int col = 0;	// Column number
	/* Print number */
	CString	strNum;
	strNum.Format("%d", pkt.num);

	UINT mask = LVIF_PARAM | LVIF_TEXT;
	
	// The protocol field is used in OnCustomdrawList1()
	row = m_listCtrlPacketList.InsertItem(mask, m_listCtrlPacketList.GetItemCount(), strNum, 0, 0, 0, (LPARAM)&(pkt.protocol));
	

	/* Print Time */
	CTime pktArrivalTime( (time_t)(pkt.header->ts.tv_sec) ) ;
	CString strPktArrivalTime = pktArrivalTime.Format("%Y/%m/%d %H:%M:%S");
	m_listCtrlPacketList.SetItemText(row, ++col, strPktArrivalTime);

	/* Print agreement */	
	if (!pkt.protocol.IsEmpty())
		m_listCtrlPacketList.SetItemText(row, ++col, pkt.protocol);
	else
		++col;

	/* Print length */
	CString strCaplen;
	strCaplen.Format("%d", pkt.header->caplen);
	m_listCtrlPacketList.SetItemText(row, ++col, strCaplen);

	/* Print source and destination MAC addresses */
	if (pkt.ethh != NULL)
	{
		CString strSrcMAC = MACAddr2CString(pkt.ethh->srcaddr);
		CString strDstMAC = MACAddr2CString(pkt.ethh->dstaddr);

		m_listCtrlPacketList.SetItemText(row, ++col, strSrcMAC);
		m_listCtrlPacketList.SetItemText(row, ++col, strDstMAC);
	}
	else
	{
		col += 2;
	}

	/* Print source and destination IP addresses */
	if (pkt.iph != NULL)
	{
		CString strSrcIP = IPAddr2CString(pkt.iph->srcaddr);
		CString strDstIP = IPAddr2CString(pkt.iph->dstaddr);

		m_listCtrlPacketList.SetItemText(row, ++col, strSrcIP);
		m_listCtrlPacketList.SetItemText(row, ++col, strDstIP);
	}
	else
	{
		col += 2;
	}
	return 0;
}

/**
*	@brief	Print packet summary information to a list control
*	@param	pool packet pool
*	@return	>=0 Number of packets in the packet pool -1 Printing failed
*/
int CkendoUIDlg::printListCtrlPacketList(PacketPool &pool)
{
	if (pool.isEmpty())
		return -1;
	int pktNum = pool.getSize();
	for (int i = 1; i <= pktNum; ++i)
		printListCtrlPacketList(pool.get(i));

	return pktNum;
}

/**
*	@brief	Traverse the data packet list and print the data packets to the list control according to the filter name
*	@param	packetLinkList packet link list
*	@param	filter	filter name
*	@return	>=0 Number of filtered data packets -1 Printing failed
*/
int CkendoUIDlg::printListCtrlPacketList(PacketPool &pool, const CString &filter)
{
	if (pool.isEmpty() || filter.IsEmpty())
		return -1;
		
	int pktNum = pool.getSize();
	int filterPktNum = 0;
	for (int i = 0; i < pktNum; ++i)
	{
		const Packet &pkt = pool.get(i);// BUG: May be Here
		if (pkt.protocol == filter)
		{
			printListCtrlPacketList(pkt);
			++filterPktNum;
		}
	}
	return filterPktNum;
}

/**
*	@brief Print the packet byte stream to the edit box (hexadecimal data area)
*	@param	pkt	data pack
*	@return 0 Printing successful -1 Printing failed
*/
int CkendoUIDlg::printEditCtrlPacketBytes(const Packet & pkt)
{
	if (pkt.isEmpty())
	{
		return -1;
	}

	CString strPacketBytes, strTmp;
	u_char* pHexPacketBytes = pkt.pkt_data;
	u_char* pASCIIPacketBytes = pkt.pkt_data;
	for (int byteCount = 0,  byteCount16=0, offset = 0; byteCount < pkt.header->caplen && pHexPacketBytes != NULL; ++byteCount)
	{
		/* If the current byte is the beginning of the line, print the offset of the beginning of the line */
		if (byteCount % 16 == 0)
		{
			strTmp.Format("%04X:", offset);
			strPacketBytes += strTmp + " ";
		}

		/* Print hexadecimal bytes */
		strTmp.Format("%02X", *pHexPacketBytes);
		strPacketBytes += strTmp + " ";
		++pHexPacketBytes;
		++byteCount16;
		
		switch (byteCount16)
		{
		case 8:
		{
			/* Print a tab every 8 bytes read */
			strPacketBytes += "\t";
			//strPacketBytes += "#";
		}
		break;
		case 16:
		{
			/* Every 16 bytes read, the ASCII character of the corresponding byte is printed. Only alphanumeric characters are printed. */
			if (byteCount16 == 16)
			{
				strPacketBytes += " ";
				for (int charCount = 0; charCount < 16; ++charCount, ++pASCIIPacketBytes)
				{
					strTmp.Format("%c", isalnum(*pASCIIPacketBytes) ? *pASCIIPacketBytes : '.');
					strPacketBytes += strTmp;
				}
				strPacketBytes += "\r\n";
				offset += 16;
				byteCount16 = 0;
			}
		}
		break;
		default:break;
		}
	}
	/* If the total length of the data packet is not aligned to 16 bytes, print the ASCII character corresponding to the last line of bytes. */
	if (pkt.header->caplen % 16 != 0)
	{
		/* Space padding ensures 16-byte alignment of the byte stream */
		for (int spaceCount = 0, byteCount16 = (pkt.header->caplen % 16); spaceCount < 16 - (pkt.header->caplen % 16); ++spaceCount)
		{
			strPacketBytes += "  ";
			strPacketBytes += " ";
			++byteCount16;
			if (byteCount16 == 8)
			{
				strPacketBytes += "\t";
				//strPacketBytes += "#";
			}
		}
		strPacketBytes += " ";
		/* Print the ASCII characters corresponding to the last line of bytes */
		for (int charCount = 0; charCount < (pkt.header->caplen % 16); ++charCount, ++pASCIIPacketBytes)
		{
			strTmp.Format("%c", isalnum(*pASCIIPacketBytes) ? *pASCIIPacketBytes : '.');
			strPacketBytes += strTmp;
		}
		strPacketBytes += "\r\n";
	}
	
	m_editCtrlPacketBytes.SetWindowTextA(strPacketBytes);

	return 0;
}

/**
*	@brief	Print the packet header parsing results to the tree control
*	@param	pkt	data pack
*	@return	0 Print successfully -1 print
*/
int CkendoUIDlg::printTreeCtrlPacketDetails(const Packet &pkt)
{
	if (pkt.isEmpty())
		return -1;

	m_treeCtrlPacketDetails.DeleteAllItems();

	/* ½¨Á¢±àºÅ½áµã */
	CString strText;

	CTime pktArrivalTime((time_t)(pkt.header->ts.tv_sec));
	CString strPktArrivalTime = pktArrivalTime.Format("%Y/%m/%d %H:%M:%S");

	strText.Format("%d data package（%s, %hu byte, capture %hu byte）",  pkt.num, strPktArrivalTime, pkt.header->len, pkt.header->caplen);

	HTREEITEM rootNode = m_treeCtrlPacketDetails.InsertItem(strText, TVI_ROOT);
	if (pkt.ethh != NULL)
	{
		printEthernet2TreeCtrl(pkt, rootNode);
	}

	m_treeCtrlPacketDetails.Expand(rootNode, TVE_EXPAND);
	return 0;
}

/**
*	@brief	Print Ethernet frame header to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insert
*/
int CkendoUIDlg::printEthernet2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode)
{
	if (pkt.isEmpty() || pkt.ethh == NULL || parentNode == NULL)
	{
		return -1;
	}
	/* Get the source and destination MAC addresses */
	CString strSrcMAC = MACAddr2CString(pkt.ethh->srcaddr);
	CString	strDstMAC = MACAddr2CString(pkt.ethh->dstaddr);
	CString strEthType;
	strEthType.Format("0x%04X", ntohs(pkt.ethh->eth_type));

	HTREEITEM	EthNode = m_treeCtrlPacketDetails.InsertItem("Ethernet (" + strSrcMAC + " -> " + strDstMAC + ") ", parentNode, 0);

	m_treeCtrlPacketDetails.InsertItem("destination MAC address: " + strDstMAC, EthNode, 0);
	m_treeCtrlPacketDetails.InsertItem("MAC address：" + strSrcMAC, EthNode, 0);
	m_treeCtrlPacketDetails.InsertItem("Type:" + strEthType, EthNode, 0);

	if (pkt.iph != NULL)
	{
		printIP2TreeCtrl(pkt, parentNode);
	}
	else if (pkt.arph != NULL)
	{
		printARP2TreeCtrl(pkt, parentNode);
	}
	return 0;
}

/**
*	@brief	Print IP header to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insertion failed
*/
int CkendoUIDlg::printIP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.iph == NULL || parentNode == NULL)
		return -1;

	HTREEITEM IPNode = m_treeCtrlPacketDetails.InsertItem("IP (" + IPAddr2CString(pkt.iph->srcaddr) + " -> " + IPAddr2CString(pkt.iph->dstaddr) + ")", parentNode, 0);
	CString strText;

	strText.Format("version number：", pkt.iph->ver_headerlen >> 4);
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText.Format("Head length：%d bytes（%d）", pkt.getIPHeaderLegnth(), pkt.getIPHeaderLengthRaw());
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText.Format("service quality: 0x%02X", pkt.iph->tos);
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText.Format("total length: %hu", ntohs(pkt.iph->totallen));
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText.Format("logo: 0x%04hX (%hu)", ntohs(pkt.iph->identifier), ntohs(pkt.iph->identifier));
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
				
	strText.Format("logo：0x%02X", pkt.getIPFlags());
	HTREEITEM IPFlagNode = m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText = "RSV: 0";
	m_treeCtrlPacketDetails.InsertItem(strText, IPFlagNode, 0);
	
	strText.Format("DF: %d", pkt.getIPFlagDF());
	m_treeCtrlPacketDetails.InsertItem(strText, IPFlagNode, 0);

	strText.Format("MF: %d", pkt.getIPFlagsMF());
	m_treeCtrlPacketDetails.InsertItem(strText, IPFlagNode, 0);
	
	strText.Format("film offset：%d", pkt.getIPOffset());
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText.Format("TTL：%u", pkt.iph->ttl);
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	switch(pkt.iph->protocol)
	{
	case PROTOCOL_ICMP:	strText = "protocol: ICMP（1）";	break;
	case PROTOCOL_TCP:	strText = "protocol：TCP（6）";	break;
	case PROTOCOL_UDP:	strText = "Protocol：UDP（17）";	break;
	default:			strText.Format("Protocol: Unknown (%d)", pkt.iph->protocol);	break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText.Format("Checksum: 0x%02hX", ntohs(pkt.iph->checksum));
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText = "Source IP address : " + IPAddr2CString(pkt.iph->srcaddr);
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	strText = "Destination IP address:" + IPAddr2CString(pkt.iph->dstaddr);
	m_treeCtrlPacketDetails.InsertItem(strText, IPNode, 0);
	
	if (pkt.icmph != NULL)
	{
		printICMP2TreeCtrl(pkt, parentNode);
	}
	else if (pkt.tcph != NULL)
	{
		printTCP2TreeCtrl(pkt, parentNode);
	}
	else if (pkt.udph != NULL)
	{
		printUDP2TreeCtrl(pkt, parentNode);
	}
	return 0;
}

/**
*	@brief	Print ARP header to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insertion failed
*/
int CkendoUIDlg::printARP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.arph == NULL || parentNode == NULL)
		return -1;

	HTREEITEM ARPNode;
	CString strText, strTmp;

	switch(ntohs(pkt.arph->opcode))
	{
	case ARP_OPCODE_REQUET:	strText.Format("ARP (request)");	break;
	case ARP_OPCODE_REPLY:	strText.Format("ARP (response)");	break;
	default:				strText.Format("ARP");			break;
	}	
	ARPNode= m_treeCtrlPacketDetails.InsertItem(strText, 0, 0, parentNode, 0);
	
	strText.Format("Hardware type:", ntohs(pkt.arph->hwtype));
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	strText.Format("Protocol type: 0x%04hx (%hu)", ntohs(pkt.arph->ptype), ntohs(pkt.arph->ptype));
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	strText.Format("Hardware address length: %u", pkt.arph->hwlen);
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	strText.Format("Protocol address length: %u", pkt.arph->plen);
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	switch(ntohs(pkt.arph->opcode))
	{
	case ARP_OPCODE_REQUET:	strText.Format("OP code: Request (%hu)", ntohs(pkt.arph->opcode));	break;
	case ARP_OPCODE_REPLY:	strText.Format("OP code: response (%hu)", ntohs(pkt.arph->opcode));	break;
	default:				strText.Format("OP code: unknown (%hu)", ntohs(pkt.arph->opcode));	break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	strText = "Source MAC address:" + MACAddr2CString(pkt.arph->srcmac);
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	strText = "Source IP address:" + IPAddr2CString(pkt.arph->srcip);
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
			
	strText = "Destination MAC address:" + MACAddr2CString(pkt.arph->dstmac);
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	strText = "Destination IP address:" + IPAddr2CString(pkt.arph->dstip);
	m_treeCtrlPacketDetails.InsertItem(strText, ARPNode, 0);
	
	return 0;
}

/**
*	@brief	Print ICMP header to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insert
*/
int CkendoUIDlg::printICMP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.icmph == NULL || parentNode == NULL)
		return -1;

	HTREEITEM ICMPNode;
	CString strText, strTmp;
	
	strText = "ICMP";
	switch(pkt.icmph->type)
	{
		case ICMP_TYPE_ECHO_REPLY:					strTmp = "(response response report)";		break;
		case ICMP_TYPE_DESTINATION_UNREACHABLE:		strTmp = "(unreachable report)";		break;
		case ICMP_TYPE_SOURCE_QUENCH:				strTmp = "(Source Suppression Report)";		break;
		case ICMP_TYPE_REDIRECT:					strTmp = "(redirect report)";			break;
		case ICMP_TYPE_ECHO:						strTmp = "(Respond to request reports)";		break;
		case ICMP_TYPE_ROUTER_ADVERTISEMENT:		strTmp = "(Router Advertisement Report)";		break;
		case ICMP_TYPE_ROUTER_SOLICITATION:			strTmp = "(Router Inquiry Report)";		break;
		case ICMP_TYPE_TIME_EXCEEDED:				strTmp = "(timeout report)";			break;
		case ICMP_TYPE_PARAMETER_PROBLEM:			strTmp = "(datagram parameter error reporting)";	break;
		case ICMP_TYPE_TIMESTAMP:					strTmp = "(timestamp request report)";		break;
		case ICMP_TYPE_TIMESTAMP_REPLY:				strTmp = "timestamp response report";		break;
		default:									strTmp.Format("(unknown)");			break;
	}
	strText += strTmp;
	ICMPNode = m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);
	
	IP_Address addr = *(IP_Address*)&(pkt.icmph->others);
	u_short id = pkt.getICMPID();
	u_short seq = pkt.getICMPSeq();
	
	strText.Format("type: %u", pkt.icmph->type);
	m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

	switch(pkt.icmph->type)
	{
		case ICMP_TYPE_ECHO_REPLY:
		{
			strText = "Code: 0";
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			strText.Format("Checksum: 0x%04hX", ntohs(pkt.icmph->chksum));
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			strText.Format("Identification: %hu", id);
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			strText.Format("Serial number:%hu", seq);
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			break;
		}


		case ICMP_TYPE_DESTINATION_UNREACHABLE: 
			strText = "Code: ";
			switch(pkt.icmph->code)
			{
				case ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_NET_UNREACHABLE: 
					strText.Format("Network unreachable (%d)", pkt.icmph->code);
					break;

				case ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_HOST_UNREACHABLE: 
					strText.Format("Host unreachable (%d)", pkt.icmph->code);
					break;

				case ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_PROTOCOL_UNREACHABLE: 
					strText.Format("The protocol is unreachable (%d)", pkt.icmph->code);
					break;

				case ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_PORT_UNREACHABLE: 
					strText.Format("Port unreachable (%d)", pkt.icmph->code);
					break;

				case 6: 
					strTmp = "Network is unknown (6)"; 
					break;

				case 7: 
					strTmp = "Trustee host unknown (7)"; 
					break;

				default: 
					strText.Format("Unknown (%d)", pkt.icmph->code); break;
			}
			strText += strTmp;
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
	
			strText.Format("Checksum: 0x%04hX", ntohs(pkt.icmph->chksum));
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
			break;
	
		case ICMP_TYPE_SOURCE_QUENCH : 
			strText.Format("Code: %d", ICMP_TYPE_SOURCE_QUENCH_CODE);
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
				
			strText.Format("Checksum: 0x%04hX", ntohs(pkt.icmph->chksum));
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
			break;
	
		case ICMP_TYPE_REDIRECT: 
				strText = "Code:";
				switch(pkt.icmph->code)
				{
				case ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_NETWORK:	
					strText.Format("Redirect to a specific network (%d)", pkt.icmph->code);
					break;

				case ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_HOST: 
					strText.Format("Redirect to specific host (%d)", pkt.icmph->code);
					break;

				case ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_TOS_AND_NETWORK: 
					strText.Format("Redirect to a specific network based on the specified service type (%d)", pkt.icmph->code);
					break;

				case ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_TOS_AND_HOST: 
					strText.Format("Redirect to a specific host based on the specified service type (%d)", pkt.icmph->code); 
					break;
				}
				strText += strTmp;
				m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
	
				strText.Format("Checksum: 0x%04hx", ntohs(pkt.icmph->chksum));
				m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
	
				strText = "Destination router IP address:" + IPAddr2CString(addr);
				m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
				break;

		case ICMP_TYPE_ECHO:
			strText.Format("Code: %d", pkt.icmph->code);
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			strText.Format("Checksum: 0x%04hX", ntohs(pkt.icmph->chksum));
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			strText.Format("Identification: %hu", id);
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);

			strText.Format("Serial number: %hu", seq);
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
			break;

		case ICMP_TYPE_TIME_EXCEEDED: 
			strText = "Code:";
			switch(pkt.icmph->code)
			{
				case ICMP_TYPE_TIME_EXCEEDED_CODE_TTL_EXCEEDED_IN_TRANSIT: 
					strText.Format("TTL timeout (%d)", pkt.icmph->code);	
					break;
				case ICMP_TYPE_TIME_EXCEEDED_CODE_FRAGMENT_REASSEMBLY_TIME_EXCEEDE: 
					strText.Format("Shard reorganization timeout (%d)", pkt.icmph->code);
					break;
			}
			strText += strTmp;
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
	
			strText.Format("Checksum: 0x%04hx", ntohs(pkt.icmph->chksum));
			m_treeCtrlPacketDetails.InsertItem(strText, ICMPNode, 0);
	
			break;
	
		default: 
			strText.Format("Code: %d", pkt.icmph->code);
			m_treeCtrlPacketDetails.InsertItem(strText, 0, 0, ICMPNode, 0);

			strText.Format("Checksum: 0x%04hX", pkt.icmph->chksum);
			m_treeCtrlPacketDetails.InsertItem(strText, 0, 0, ICMPNode, 0);

			break;
		}
	return 0;
}

/**
*	@brief	Print TCP headers to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insertion failed
*/
int CkendoUIDlg::printTCP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.tcph == NULL || parentNode == NULL)
	{
		return -1;
	}
	HTREEITEM TCPNode;
	CString strText, strTmp;
							
	strText.Format("TCP (%hu -> %hu)", ntohs(pkt.tcph->srcport), ntohs(pkt.tcph->dstport));
	TCPNode = m_treeCtrlPacketDetails.InsertItem(strText,parentNode, 0);
							
	strText.Format("Source port: %hu", ntohs(pkt.tcph->srcport));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Destination port: %hu", ntohs(pkt.tcph->dstport));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Serial number: 0x%0lX", ntohl(pkt.tcph->seq));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Confirmation number: 0x%0lX", ntohl(pkt.tcph->ack));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Header length: %d bytes (%d)", pkt.getTCPHeaderLength(), pkt.getTCPHeaderLengthRaw());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Flag: 0x%03X", pkt.getTCPFlags(), pkt.getTCPFlags());
	HTREEITEM TCPFlagNode = m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("URG：%d", pkt.getTCPFlagsURG());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPFlagNode, 0);
							
	strText.Format("ACK：%d", pkt.getTCPFlagsACK());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPFlagNode, 0);
							
	strText.Format("PSH：%d", pkt.getTCPFlagsPSH());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPFlagNode, 0);
							
	strText.Format("RST：%d", pkt.getTCPFlagsRST());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPFlagNode, 0);

	strText.Format("SYN：%d", pkt.getTCPFlagsSYN());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPFlagNode, 0);
							
	strText.Format("FIN：%d", pkt.getTCPFlagsFIN());
	m_treeCtrlPacketDetails.InsertItem(strText, TCPFlagNode, 0);
							 
	strText.Format("Window size: %hu", ntohs(pkt.tcph->win_size));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Checksum: 0x%04hX", ntohs(pkt.tcph->chksum));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);
							
	strText.Format("Emergency pointer: %hu", ntohs(pkt.tcph->urg_ptr));
	m_treeCtrlPacketDetails.InsertItem(strText, TCPNode, 0);

	if (pkt.dnsh != NULL)
	{
		printDNS2TreeCtrl(pkt, parentNode);
	}
	else if (pkt.dhcph != NULL)
	{
		printDHCP2TreeCtrl(pkt, parentNode);
	}
	else if (pkt.httpmsg != NULL)
	{
		printHTTP2TreeCtrl(pkt, parentNode);
	}

	return 0;
}

/**
*	@brief	Print UDP header to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insertion failed
*/
int CkendoUIDlg::printUDP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.udph == NULL || parentNode == NULL)
	{
		return -1;
	}
	HTREEITEM UDPNode;		
	CString strText, strTmp;
							
	strText.Format("UDP（%hu -> %hu）", ntohs(pkt.udph->srcport), ntohs(pkt.udph->dstport));
	UDPNode = m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);
							
	strText.Format("Source port: %hu", ntohs(pkt.udph->srcport));
	m_treeCtrlPacketDetails.InsertItem(strText, UDPNode, 0);
							
	strText.Format("Destination port: %hu", ntohs(pkt.udph->dstport));
	m_treeCtrlPacketDetails.InsertItem(strText, UDPNode, 0);
							
	strText.Format("Length: %hu", ntohs(pkt.udph->len));
	m_treeCtrlPacketDetails.InsertItem(strText, UDPNode, 0);
							
	strText.Format("Checksum: 0x%04hX", ntohs(pkt.udph->checksum));
	m_treeCtrlPacketDetails.InsertItem(strText, UDPNode, 0);

	if (pkt.dnsh != NULL)
	{
		//printDNS2TreeCtrl(pkt, parentNode);
	}
	else if (pkt.dhcph != NULL)
	{
		printDHCP2TreeCtrl(pkt, parentNode);
	}
	return 0;
}

/** 
*	@brief	Print DNS node title
*	@param	pkt	data pack
*	@param	parentNode parent node
*	@return DNS node
*/
HTREEITEM CkendoUIDlg::printDNSBanner(const Packet &pkt, HTREEITEM &parentNode)
{
	if (pkt.isEmpty() || parentNode == NULL)
	{
		return NULL;
	}
	CString strText;

	switch (pkt.getDNSFlagsQR())
	{
	case DNS_FLAGS_QR_REQUEST:	strText = "DNS (request)";		break;
	case DNS_FLAGS_QR_REPLY:	strText = "DNS (response)";		break;
	}
	return m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);
}


/**
*	@brief	Print DNS header
*	@param	pkt	data pack
*	@param	parentNode	parent node
*	@return 0 Printing successful -1 Printing failed
*/
int CkendoUIDlg::printDNSHeader(const Packet &pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.dnsh == NULL || parentNode == NULL)
	{
		return -1;
	}
	CString strText, strTmp;
	strText.Format("Identification: 0x%04hX (%hu)", ntohs(pkt.dnsh->identifier), ntohs(pkt.dnsh->identifier));
	m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);

	strText.Format("Flag: 0x%04hX", ntohs(pkt.dnsh->flags));
	strText += strTmp;

	HTREEITEM DNSFlagNode = m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);
	/* flag subfield */
	switch (pkt.getDNSFlagsQR())
	{
	case DNS_FLAGS_QR_REQUEST:	strText = "QR:; Query message (0)";	break;
	case DNS_FLAGS_QR_REPLY:	strText = "QR:; response message (1)";	break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);

	switch (pkt.getDNSFlagsOPCODE())
	{
	case DNS_FLAGS_OPCODE_STANDARD_QUERY:			strText = "OPCODE: Standard Query (0)";			break;
	case DNS_FLAGS_OPCODE_INVERSE_QUERY:			strText = "OPCODE: reverse query (1)";			break;
	case DNS_FLAGS_OPCODE_SERVER_STATUS_REQUEST:	strText = "OPCODE: Server status request (2)";	break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);

	switch (pkt.getDNSFlagsAA())
	{
	case 0:	strText = "AA: Unauthorized answer (0)";	break;
	case 1: strText = "AA: Authorized Answer (1)";		break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);


	switch (pkt.getDNSFlagsTC())
	{
	case 0: strText = "TC: The message is not truncated (0)";	break;
	case 1: strText = "TC: Message truncation (1)";		break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);


	switch (pkt.getDNSFlagsRD())
	{
	case 0: strText = "RD: 0";						break;
	case 1: strText = "RD: Want to perform recursive query (1)";	break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);

	switch (pkt.getDNSFlagsRA())
	{
	case 0: strText = "RA: The server does not support recursive queries (0)"; break;
	case 1: strText = "RA: Server supports recursive queries (1)";	break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);

	strText.Format("Z: reserved (%d)", pkt.getDNSFlagsZ());
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);

	switch (pkt.getDNSFlagsRCODE())
	{
	case DNS_FLAGS_RCODE_NO_ERROR:			strText = "RCODE: No errors (0)";							 break;
	case DNS_FLAGS_RCODE_FORMAT_ERROR:		strText = "RCODE: format error (1)";							 break;
	case DNS_FLAGS_RCODE_SERVER_FAILURE:	strText = "RCODE: DNS server issues (2)";						 break;
	case DNS_FLAGS_RCODE_NAME_ERROR:		strText = "RCODE: The domain name does not exist or is wrong (3)";					 break;
	case DNS_FLAGS_RCODE_NOT_IMPLEMENTED:	strText = "RCODE: Query type not supported (4)";					 break;
	case DNS_FLAGS_RCODE_REFUSED:			strText = "RCODE: Administratively prohibited (5)";						 break;
	default:								strText.Format("RCODE: reserved (%d)", pkt.getDNSFlagsRCODE()); break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DNSFlagNode, 0);

	strText.Format("Number of query records: %hu", ntohs(pkt.dnsh->questions));
	m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);

	strText.Format("Number of answer records: %hu", ntohs(pkt.dnsh->answer_RRs));
	m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);

	strText.Format("Number of authorized answer records: %hu", ntohs(pkt.dnsh->authority_RRs));
	m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);

	strText.Format("Number of additional information records: %hu", ntohs(pkt.dnsh->additional_RRs));
	m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);

	return 0;
}

/**
*	@brief	Convert the type field in the DNS message into a CString class string
*	@param	type	The type field in the DNS message
*	@return CString string
*/
CString CkendoUIDlg::DNSType2CString(const u_short &type)
{
	CString strType;
	switch (ntohs(type))
	{
	case DNS_TYPE_A:		strType = "Type A";										break;
	case DNS_TYPE_NS:		strType = "Type NS";									break;
	case DNS_TYPE_CNAME:	strType = "Type CNAME";									break;
	case DNS_TYPE_SOA:		strType = "Type SOA";									break;
	case DNS_TYPE_PTR:		strType = "Type PTR";									break;
	case DNS_TYPE_MX:		strType = "Type MX";									break;
	case DNS_TYPE_AAAA:		strType = "Type AAAA";									break;
	case DNS_TYPE_ANY:		strType = "Type ANY";									break;
	default:				strType.Format("Type unknown (%hu)", ntohs(type));		break;
	}
	return strType;
}

/**
*	@brief	Convert the class field in the DNS message into a CString class string
*	@param	class The class field in the DNS message
*	@return CString string
*/
CString CkendoUIDlg::DNSClass2CString(const u_short &classes)
{
	CString strClass;
	switch (ntohs(classes))
	{
	case DNS_CLASS_IN:		strClass = "Class IN";									break;
	case DNS_CLASS_CS:		strClass = "Class CS";									break;
	case DNS_CLASS_HS:		strClass = "Class HS";									break;
	default:				strClass.Format("Class unknown (%hu)", ntohs(classes));	break;
	}
	return strClass;
}


/**
*	@brief	Print DNS query part
*	@param	DNSQuery	Query part
*	@param	questions	Query the number of records
*	@param	parentNode	parent node
*	@return	0 Printing successful -1 Printing failed Positive integer DNS query
*/
int CkendoUIDlg::printDNSQuery(char *DNSQuery, const u_short &questions, HTREEITEM &parentNode)
{
	if (DNSQuery == NULL && parentNode == NULL)
	{
		return -1;
	}
	CString strText, strTmp;
	HTREEITEM DNSQueryNode = m_treeCtrlPacketDetails.InsertItem("Query part: ", parentNode, 0);

	/* Query part */
	
	char *p = DNSQuery;
	//if (questions < 10)
	//{
		for(int queryNum = 0; queryNum < questions; ++queryNum)
		{
			char *name = (char*)malloc(strlen(p) + 1);
			translateNameInDNS(name, p);

			/* Skip domain name field */
			p += strlen(p) + 1;
			strText.Format("%s: ", name);

			DNS_Query *DNSQuery = (DNS_Query*)p;
			strText += DNSType2CString(DNSQuery->type) + ", ";
			strText += DNSClass2CString(DNSQuery->classes);
			m_treeCtrlPacketDetails.InsertItem(strText, DNSQueryNode, 0);

			/* Skip query type and query class fields */
			p += sizeof(DNS_Query);		
			free(name);
		}// for
	//}// if
	return p - DNSQuery + 1;
}

/**
*	@brief	Print DNS answer section
*	@param	dnsa		Answer section
*	@param	answers		Number of answer records
*	@param	parentNode	parent node
*	@return	0 Printing successful -1 Printing failed Positive integer DNS response part
*/
//int printDNSAnswer(char *DNSAnswer, const u_short &answers, const DNS_Header *dnsh, HTREEITEM &parentNode)
//{
//	if (DNSAnswer == NULL || parentNode == NULL)
//	{
//		return -1;
//	}
//	CString strText, strTmp;
//	HTREEITEM DNSAnswerNode = m_treeCtrlPacketDetails.InsertItem("Answer part:", parentNode, 0);
//
//	int answerNum = 0, byteCounter = 0;
//	char *p = DNSAnswer;
//	/* Answer section */
//	while (answerNum < answers)
//	{
//		/* pointer */
//		if (*p == 0xc0)
//		{
//
//			/* Point to offset
//			++p;
//
//			char *name = (char*)(pkt_data + offset + *(char*)p);			// domain name
//			char *name1 = (char*)malloc(strlen(name)+1);
//
//
//			translateName(name1, name);
//
//			strText.Format("%s", name1);
//			strText += "£º";
//
//			free(name1);
//			*/
//
//			char name[70];
//			char name1[70];
//
//			translateData(dnsh, name, p, 2);
//			translateName(name1, name);
//
//			strText.Format("%s:", name1);
//
//			/* Point to offset */
//			++p;
//			++byteCounter;
//
//
//			/* pointing type */
//			++p;
//			++byteCounter;
//			DNS_ResourceRecord *dnsa = (DNS_ResourceRecord*)p;
//
//			u_short type = ntohs(dnsa->type);
//			u_short classes = ntohs(dnsa->classes);
//			u_long  ttl = ntohl(dnsa->ttl);
//
//			switch (type)
//			{
//			case 1:	strTmp = "type A"; break;
//			case 2:	strTmp = "type NS"; break;
//			case 5: strTmp = "type CNAME"; break;
//			case 6: strTmp = "type SOA"; break;
//			case 12: strTmp = "type PTR"; break;
//			case 15: strTmp = "type MX"; break;
//			case 28: strTmp = "type AAAA"; break;
//			case 255: strTmp = "type ANY"; break;
//			default: strTmp.Format("type UNKNOWN(%hu)", type); break;
//			}
//			strText += strTmp + ", ";
//
//			switch (classes)
//			{
//			case 1: strTmp = "class INTERNET"; break;
//			case 2: strTmp = "class CSNET";	break;
//			case 3: strTmp = "class COAS";	break;
//			default: strTmp.Format("class UNKNOWN(%hu)", classes); break;
//			}
//			strText += strTmp + ", ";
//
//			strTmp.Format("TTL %lu", ttl);
//			strText += strTmp + ", ";
//
//			/* Points to resource data length */
//			p += sizeof(DNS_ResourceRecord);
//			byteCounter += sizeof(DNS_ResourceRecord);
//			u_short data_len = ntohs(*(u_short*)p);
//
//			strTmp.Format("Resource data length %hu", data_len);
//			strText += strTmp + ", ";
//
//			/* Point to resource data */
//			p += sizeof(u_short);
//			byteCounter += sizeof(u_short);
//
//			/* Query resource data of type NS, CNAME, PTR */
//			if (type == 2 || type == 5 || type == 12)
//			{
//
//				/* Resource data is pointer 0xc0 + offset */
//				if (*(char*)p == 0xC0)
//				{
//					/* Get data based on offset
//					char *data = (char*)(pkt_data + offset + *(char*)(p+1));			// ÓòÃû
//					char *data1 = (char*)malloc(strlen(data)+1);
//
//					translateName(data1, data);
//
//					strText.Format("%s", data1);
//					strText += strTmp;
//
//					free(data1);
//					*/
//					char data[70];
//					char data1[70];
//
//					translateData(dnsh, data, p, 2);
//					translateName(data1, data);
//
//					strText.Format("%s", data1);
//					strText += strTmp;
//
//				}
//				/* Resource data exists at pointer 0xc0 + offset */
//				else if (isNamePtr(p))
//				{
//					char data[70];
//					char data1[70];
//
//					translateData(dnsh, data, p, data_len);		// Remove pointer 0xc0+offset
//					translateName(data1, data);								// È¥µô'.'
//
//					strTmp.Format("%s", data1);
//					strText += strTmp;
//				}
//				/* Pointer 0xc0 + offset does not exist in resource data */
//				else
//				{
//					char *data = (char*)malloc(data_len);
//
//					translateName(data, p);
//
//					strTmp.Format("%s", data);
//					strText += strTmp;
//					free(data);
//
//				}
//			}
//			/* Query resource data of type A */
//			else if (type == 1)
//			{
//				IP_Address data = *(IP_Address*)p;
//				strText += IPAddr2CString(data);
//			}
//
//			m_treeCtrlPacketDetails.InsertItem(strText, DNSAnswerNode, 0);
//
//			/* Skip data part*/
//			p += data_len;
//			byteCounter += data_len;
//
//		}// if
//		answerNum++;
//	}// while
//	return byteCounter;
//}

/**
*	@brief	Print DNS resource records
*	@param	DNSResourceRecord	resource record
*	@param	resourceRecordNum	Number of resource records
*	@param	resourceRecordType	Resource record type (answer, authorized answer, additional information)
*	@param	pDNSHeader			DNS header
*	@param	parentNode			parent node
*	@return	0 Printing successful -1 Printing failed Positive integer Total DNS resource records
*/
int CkendoUIDlg::printDNSResourceRecord(char *DNSResourceRecord, const u_short &resourceRecordNum, const int &resourceRecordType ,const DNS_Header *pDNSHeader, HTREEITEM parentNode)
{
	if (DNSResourceRecord == NULL || resourceRecordNum == 0 || pDNSHeader == NULL || parentNode == NULL)
	{
		return -1;
	}
	char *p = DNSResourceRecord;
	CString strText, strTmp;

	switch (resourceRecordType)
	{
	case DNS_RESOURCE_RECORD_TYPE_ANSWER:		strText = "Answer part:";		break;
	case DNS_RESOURCE_RECORD_TYPE_AUTHORITY:	strText = "Authorized answer part:";	break;
	case DNS_RESOURCE_RECORD_TYPE_ADDITIONAL:	strText = "Additional information section:";	break;
	}
	HTREEITEM DNSResourceRecordNode = m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);

	for (int count = 0; count < 1; ++count) //count < resourceRecordNum; ++count)
	{

		if ( *(u_char*)p == 0xC0)
		{
			// name
			strText = getNameInDNS(p, pDNSHeader) + ": ";

			// oriented type, class, ttl
			p += 2;			// 2 = 0xC0 + Offset
		}
		else
		{
			char *name = (char*)malloc(strlen(p) + 1);
			translateNameInDNS(name, p);

			CString strText, strTmp;
			strText.Format("%s: ", name);

			// directional type, class, ttl
			p += strlen(name) + 1;
			free(name);
		}

		DNS_ResourceRecord	*pRecord = (DNS_ResourceRecord*)p;
		strText += DNSType2CString(pRecord->type) + ", ";
		strText += DNSClass2CString(pRecord->classes) + ", ";
		strTmp.Format("TTL %d", ntohl(pRecord->ttl));
		strText += strTmp + ", ";

		// Points to resource data length
		p += sizeof(DNS_ResourceRecord);
		u_short dataLength = *(u_short*)p;
		strTmp.Format("Resource data length: %hu bytes", dataLength);
		strText += strTmp + ", ";

		// Point to resource data
		p += sizeof(u_short);

		switch (ntohs(pRecord->type))
		{
		case DNS_TYPE_A:
			strText += "IP address:" + IPAddr2CString(*(IP_Address*)p);
			break;
		case DNS_TYPE_NS:
			strText += "Name servers: " + IPAddr2CString(*(IP_Address*)p);
			break;
		case DNS_TYPE_CNAME:
		{
			//char *cname = (char*)malloc(dataLength);
			//translateNameInDNS(cname, p);

			CString strCName = getNameInDNS(p, pDNSHeader);
			strText += "Alias: " + strCName;
			//m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);
			//free(cname);
			break;
		}
		//case DNS_TYPE_SOA:
		//	strText += ;
		//	break;
		//case DNS_TYPE_PTR:
		//	strText += ;
		//	break;
		//case DNS_TYPE_AAAA:
		//	strText += ;
		//	break;
		//case DNS_TYPE_ANY:
		//	strText += ;
		//	break;
		default:
			/*strTmp.Format("Type Î´Öª(%hu),", ntohs(pRecord->type));
			strText += strTmp;*/
			break;
		}
		m_treeCtrlPacketDetails.InsertItem(strText, DNSResourceRecordNode, 0);

	}// for
	return p - DNSResourceRecord + 1;
}

/**
*	@brief	Print DNS messages to the tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insert
*/
int CkendoUIDlg::printDNS2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.dnsh == NULL || parentNode == NULL)
	{
		return -1;
	}
	HTREEITEM DNSNode = printDNSBanner(pkt, parentNode);

	printDNSHeader(pkt, DNSNode);


	char *DNSQuery = (char*)pkt.dnsh + DNS_HEADER_LENGTH;
	int DNSQueryLen = printDNSQuery(DNSQuery, ntohs(pkt.dnsh->questions), DNSNode);

	char *DNSAnswer = NULL, *DNSAuthority = NULL, *DNSAdditional = NULL;
	int DNSAnswerLen = 0, DNSAuthorityLen = 0;

	if (ntohs(pkt.dnsh->answer_RRs) > 0)
	{
		DNSAnswer = DNSQuery + DNSQueryLen;
		DNSAnswerLen = printDNSResourceRecord(DNSAnswer, ntohs(pkt.dnsh->answer_RRs), DNS_RESOURCE_RECORD_TYPE_ANSWER, pkt.dnsh, DNSNode);
	}

	if (ntohs(pkt.dnsh->authority_RRs) > 0)
	{
		DNSAuthority = DNSAnswer + DNSAnswerLen;
		DNSAuthorityLen = printDNSResourceRecord(DNSAuthority, ntohs(pkt.dnsh->authority_RRs), DNS_RESOURCE_RECORD_TYPE_AUTHORITY, pkt.dnsh, DNSNode);
	}
	

	if (ntohs(pkt.dnsh->additional_RRs) > 0)
	{
		DNSAdditional = DNSAuthority + DNSAuthorityLen;
		printDNSResourceRecord(DNSAdditional, ntohs(pkt.dnsh->additional_RRs), DNS_RESOURCE_RECORD_TYPE_ADDITIONAL, pkt.dnsh, DNSNode);
	}


	return 0;
}

/**
*	@brief	Print DHCP header to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insert
*/
int CkendoUIDlg::printDHCP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.dhcph == NULL || parentNode == NULL)
	{
		return -1;
	}

	HTREEITEM DHCPNode = m_treeCtrlPacketDetails.InsertItem("DHCP", parentNode, 0);
	CString strText, strTmp;
	/* Parse dhcp header */
	strText.Format("Message type: %d", pkt.dhcph->op);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText.Format("Hardware type: %d", pkt.dhcph->htype);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText.Format("Hardware address length: %d", pkt.dhcph->hlen);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

	strText.Format("Hop count: %d",pkt.dhcph->hops);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText.Format("Transaction ID: 0x%08lX", ntohl(pkt.dhcph->xid));
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText.Format("Client startup time: %hu", ntohs(pkt.dhcph->secs));
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText.Format("Flag: 0x%04hX", ntohs(pkt.dhcph->flags));
	switch(ntohs(pkt.dhcph->flags) >> 15)
	{
	case DHCP_FLAGS_BROADCAST: strText += "(broadcast)"; break;
	case DHCP_FLAGS_UNICAST: strText += "(Unicast)"; break;
	}
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText = "Client IP address:" + IPAddr2CString(pkt.dhcph->ciaddr);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText = "Your (customer) IP address:" + IPAddr2CString(pkt.dhcph->yiaddr);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText = "Server IP address:" + IPAddr2CString(pkt.dhcph->siaddr);;
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText = "Gateway IP address:" + IPAddr2CString(pkt.dhcph->giaddr);
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	/* Parse the remainder of the dhcp header */
	CString strChaddr;
	for (int i=0; i< 6; ++i)
	{
		strTmp.Format("%02X", pkt.dhcph->chaddr[i]);
		strChaddr += strTmp + "-";
	}
	strChaddr.Delete(strChaddr.GetLength() - 1, 1);

	strText = "Client MAC address:" + strChaddr;
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText = "Server hostname:";
	strTmp.Format("%s", pkt.dhcph->snamer);
	strText += strTmp;
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	strText = "Boot file name:";
	strTmp.Format("%s", pkt.dhcph->file);
	strText += strTmp;
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	
	// Skip boot filename
	u_char *p = (u_char*)pkt.dhcph->file + 128;
	
	if(ntohl(*(u_long*)p) == 0x63825363)
	{
		strText = "Magic cookie: DHCP";
		m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);
	}
	
	// Skip magic cookie
	p += 4;
	
	while(*p != 0xFF)
	{
		switch (*p)
		{
		case DHCP_OPTIONS_DHCP_MESSAGE_TYPE:
		{
			strText = "Option: (53) DHCP message type";
			switch (*(p + 2))
			{
			case 1: strText += "（Discover）"; break;
			case 2: strText += "（Offer）"; break;
			case 3: strText += "（Request）"; break;
			case 4: strText += "（Decline）"; break;
			case 5: strText += "（ACK）"; break;
			case 6: strText += "（NAK）"; break;
			case 7: strText += "（Release）"; break;
			case 8: strText += "（Inform）"; break;
			}
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			strText.Format("Length: %d", *(++p));
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			strText.Format("DHCP: %d", *(++p));
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			// Point to next option
			++p;
		}
			break;

		case DHCP_OPTIONS_REQUESTED_IP_ADDRESS:
		{
			strText = "Option: (50) Request IP address";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			strText.Format("Length: %d", *(++p));
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			IP_Address *addr = (IP_Address*)(++p);
			strText = "address:" + IPAddr2CString(*addr);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			// Point to next option
			p += 4;
		}
			break;

		case DHCP_OPTIONS_IP_ADDRESS_LEASE_TIME:
		{
			strText = "Option: (51) IP address lease time";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			strText.Format("Length: %d", *(++p));
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			u_int time = *(++p);
			strText.Format("Lease time: %u", time);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			// Point to next option
			p += 4;
		}
			break;

		case DHCP_OPTIONS_CLIENT_IDENTIFIER:
		{
			strText = "Option: (61) Client ID";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			strText = "Hardware type:";
			if (*(++p) == 0x01)
			{
				strText += "Ethernet (0x01)";
				m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

				MAC_Address *addr = (MAC_Address*)(++p);
				strText = "Client ID:" + MACAddr2CString(*addr);
				m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

				p += 6;
			}
			else
			{
				strText.Format("%d", *p);
				strText += strTmp;
				m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

				p += len;
			}
		}
			break;

		case DHCP_OPTIONS_VENDOR_CLASS_IDENTIFIER:
		{
			strText = "Options: (60) Supplier Class Identification";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			int count = 0;
			strText = "Supplier class identification:";
			for (; count < len; count++)
			{
				strTmp.Format("%c", *(++p));
				strText += strTmp;
			}
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			++p;
		}
			break;

		case DHCP_OPTIONS_SERVER_IDENTIFIER:
		{
			strText = "Options: (54) Server ID";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			IP_Address *addr = (IP_Address*)(++p);
			strText = "Server ID:" + IPAddr2CString(*addr);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			p += 4;
		}
			break;

		case DHCP_OPTIONS_SUBNET_MASK:
		{

		
			strText = "Options: (1) Subnet mask";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			IP_Address *submask = (IP_Address*)(++p);
			strText = "Subnet mask : " + IPAddr2CString(*submask);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			p += 4;
		}
			break;

		case DHCP_OPTIONS_ROUTER_OPTION:
		{

		
			strText = "Options: (3) Router";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			int count = 0;
			while (count < len)
			{
				IP_Address *addr = (IP_Address*)(++p);
				strText = "router: " + IPAddr2CString(*addr);
				m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

				count += 4;
				p += 4;
			}
		}
			break;

		case DHCP_OPTIONS_DOMAIN_NAME_SERVER_OPTION: 
		{
			strText = "Options: (6) DNS server";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d ", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			int count = 0;
			++p;
			while (count < len)
			{
				IP_Address *addr = (IP_Address*)(p);
				strText = "DNS server: " + IPAddr2CString(*addr);
				m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

				count += 4;
				p += 4;
			}
		}
			break;


		case DHCP_OPTIONS_HOST_NAME_OPTION:
		{
			strText = "Options: (12) Hostname";
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			int count = 0;
			strText = "CPU name:";

			for (; count < len; count++)
			{
				strTmp.Format("%c", *(++p));
				strText += strTmp;
			}
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			++p;
		}
			break;

		case DHCP_OPTIONS_PAD_OPTION:
			++p;
			break;

		default:
		{
			strText.Format("Options: (%d)", *p);
			HTREEITEM DHCPOptionNode = m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);

			int len = *(++p);
			strText.Format("Length: %d", len);
			m_treeCtrlPacketDetails.InsertItem(strText, DHCPOptionNode, 0);

			// Point to option content
			++p;

			// Skip option content
			p += len;
		}
			break;
		}// switch 
	
	}// while
	strText = "Options: (255) end";
	m_treeCtrlPacketDetails.InsertItem(strText, DHCPNode, 0);	 
		
	return 0;
}

/**
*	@brief	Print HTTP messages to tree control
*	@param	pkt data pack
*	@param	parentNode parent node
*	@return	0 Insertion successful -1 Insert
*/
int CkendoUIDlg::printHTTP2TreeCtrl(const Packet & pkt, HTREEITEM & parentNode)
{
	if (pkt.isEmpty() || pkt.httpmsg == NULL || parentNode == NULL)
	{
		return -1;
	}

	u_char *p = pkt.httpmsg;
	int HTTPMsgLen = pkt.getL4PayloadLength();
		
	CString strText;
	if (ntohs(pkt.tcph->dstport) == PORT_HTTP)
	{
		strText = "HTTP (request)";
	}
	else if (ntohs(pkt.tcph->srcport) == PORT_HTTP)
	{
		strText = "HTTP (response)";
	}
	HTREEITEM HTTPNode = m_treeCtrlPacketDetails.InsertItem(strText, parentNode, 0);
	
	for(int count = 0; count < HTTPMsgLen; )
	{
		strText = "";
		while(*p != '\r')
		{
			strText += *p;
			++p;
			++count;
		}
		strText += "\\r\\n";
		m_treeCtrlPacketDetails.InsertItem(strText, HTTPNode, 0);
	
		p += 2;
		count += 2;
	}	
	return 0;
}

/**
*	@brief	Convert MAC address to CString class string
*	@param	addr MAC address
*	@return	CString class string
*/
CString CkendoUIDlg::MACAddr2CString(const MAC_Address &addr)
{
	CString strAddr, strTmp;

	for (int i = 0; i < 6; ++i)
	{
		strTmp.Format("%02X", addr.bytes[i]);
		strAddr += strTmp + "-";
	}
	strAddr.Delete(strAddr.GetLength() - 1, 1);

	return strAddr;
}

/**
*	@brief	Convert IP address to CString class string
*	@param	 IP address
*	@return	CString class string
*/
CString CkendoUIDlg::IPAddr2CString(const IP_Address &addr)
{
	CString strAddr, strTmp;

	for (int i = 0; i < 4; ++i)
	{
		strTmp.Format("%d", addr.bytes[i]);
		strAddr += strTmp + ".";
	}
	strAddr.Delete(strAddr.GetLength() - 1, 1);

	return strAddr;
}

/**
*	@brief	Click the list to print the packet header parsing results to the tree control, and the packet byte stream to the edit control
*	@param	
*	@return	-
*/
void CkendoUIDlg::OnClickedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	/* Get the row number of the selected row */
	int selectedItemIndex = m_listCtrlPacketList.GetSelectionMark();
	CString strPktNum = m_listCtrlPacketList.GetItemText(selectedItemIndex, 0);
	int pktNum = _ttoi(strPktNum);
	if (pktNum < 1 || pktNum > m_pool.getSize())
		return;

	//POSITION pos = g_packetLinkList.FindIndex(pktNum - 1);
	//Packet &pkt = g_packetLinkList.GetAt(pos);

	const Packet &pkt = m_pool.get(pktNum);

	printTreeCtrlPacketDetails(pkt);
	printEditCtrlPacketBytes(pkt);
}

/**
*	@brief	Click the hyperlink to open Github
*	@param
*	@return -
*/
void CAboutDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}

/**
*	@brief	根据协议名给ListCtrl控件的Item填充底色
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = (LPNMLVCUSTOMDRAW)pNMHDR;
	*pResult = 0;

	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)	
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if(CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage) // Before an Item (a row) is painted
	{
		COLORREF itemColor;
		CString *pStrPktProtocol = (CString*)(pNMCD->nmcd.lItemlParam);	// Pass the protocol field of the data packet in printListCtrlPacketList(pkt)

		///* If the row is selected, adjust its background color to */
		//if (pNMCD->nmcd.uItemState & CDIS_SELECTED)
		//{
		//	pNMCD->clrTextBk = RGB(0, 0, 0);
		//}
		if (!pStrPktProtocol->IsEmpty())
		{
			if (*pStrPktProtocol == "ARP")
			{
				itemColor = RGB(255, 182, 193);	// red
			}
			else if (*pStrPktProtocol == "ICMP")
			{
				itemColor = RGB(186, 85, 211);	// Purple
			}
			else if (*pStrPktProtocol == "TCP")
			{
				itemColor = RGB(144, 238, 144);	// green
			}
			else if (*pStrPktProtocol == "UDP")
			{
				itemColor = RGB(100, 149, 237);	// blue

			}
			else if (*pStrPktProtocol == "DNS")
			{
				itemColor = RGB(135, 206, 250);	// light blue
			}
			else if (*pStrPktProtocol == "DHCP")
			{
				itemColor = RGB(189, 254, 76);	// light yellow
			}
			else if (*pStrPktProtocol == "HTTP")
			{
				itemColor = RGB(238, 232, 180);	// yellow
			}
			else
			{
				itemColor = RGB(211, 211, 211);	// grey
			}
			pNMCD->clrTextBk = itemColor;
		}		
		*pResult = CDRF_DODEFAULT;
	}
}
/*************************************************************
*
*		Custom message processing implementation
*
*************************************************************/
/**
*	@brief	Handle custom messages_PKTCATCH
*	@param	wParam	16-bit custom parameters
*	@param	lParam	32-bit custom parameters
*	@return	
*/
LRESULT CkendoUIDlg::OnPktCatchMessage(WPARAM wParam, LPARAM lParam)
{
	int pktNum = lParam;
	if (pktNum > 0)
	{
		Packet &pkt = m_pool.get(pktNum);
		/* Check whether the filter is enabled. If enabled, only new captured packets that match the filter will be printed.*/
		int selFilterIndex = m_comboBoxFilterList.GetCurSel();
		if (selFilterIndex > 0)
		{
			CString strFilter;
			m_comboBoxFilterList.GetLBText(selFilterIndex, strFilter);
			if (strFilter == pkt.protocol)
				printListCtrlPacketList(pkt);
		}
		else
			printListCtrlPacketList(pkt);

		// Modify status bar - total number of data packets, number of data packets displayed
		updateStatusBar(CString(""), m_pool.getSize(), m_listCtrlPacketList.GetItemCount());
	}

	return 0;
}
/**
*	@brief	Handle custom message WM_TEXIT
*	@param	wParam	16-bit custom parameters
*	@param	lParam	32-bit custom parameters
*	@return
*/
LRESULT CkendoUIDlg::OnTExitMessage(WPARAM wParam, LPARAM lParam)
{
	m_catcher.closeAdapter();
	return 0;
}

/**
*	@brief	Handling toolbar button tips
*	@param	-
*	@param	-
*	@return -
*/
BOOL CkendoUIDlg::OnToolTipText(UINT, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXT   *pTTT = (TOOLTIPTEXT*)pNMHDR;
	UINT  uID = pNMHDR->idFrom;     // It is equivalent to the wParam (low-order) of the original WM_COMMAND delivery method. What is placed in wParam is the ID of the control.
	if (pTTT->uFlags  &  TTF_IDISHWND)
		uID = ::GetDlgCtrlID((HWND)uID);
	if (uID == NULL)
		return   FALSE;
	switch (uID)
	{
	case ID_TOOLBARBTN_START:
		pTTT->lpszText = _T("Start capturing");
		break;

	case ID_TOOLBARBTN_STOP:
		pTTT->lpszText = _T("end capture");
		break;

	case ID_MENU_FILE_OPEN:
		pTTT->lpszText = _T("open a file");
		break;

	case ID_MENU_FILE_SAVEAS:
		pTTT->lpszText = _T("Save as");
		break;

	case ID_TOOLBARBTN_CLEAR:
		pTTT->lpszText = _T("clear filter");
		break;

	case ID_TOOLBARBTN_FILTER:
		pTTT->lpszText = _T("Apply filter");
		break;
	}

	return TRUE;
}

/**
*	@brief	Shortcut key - Ctrl + G - Get focus of the selected item in the packet list
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnAcceleratorCtrlG()
{
	m_listCtrlPacketList.SetFocus();

	/* The vertical scroll bar automatically jumps to the selected position */
	int selItemIndex = m_listCtrlPacketList.GetSelectionMark();			
	int topItemIndex = m_listCtrlPacketList.GetTopIndex();				// The index of the currently top-most visible item in the list 
	CRect rc;
	m_listCtrlPacketList.GetItemRect(selItemIndex, rc, LVIR_BOUNDS);	// Get the size rc of a row
	CSize sz(0, (selItemIndex - topItemIndex)*rc.Height());				// (selItemIndex - topItemIndex)Indicates the scroll amount n, >0 means scrolling down n lines, <0 means scrolling up n lines,
																		// *rc.Height (row height) is because scroll() scrolls by pixel value
	m_listCtrlPacketList.Scroll(sz);									// Scroll to selected position
}

/**
*	@brief	In the packet list control, use the up and down arrow keys to control the currently selected row.
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	bool selectedItemChangedFlag = false;
	int selectedItemIndex = m_listCtrlPacketList.GetSelectionMark();
	/* Determine whether the pressed key is the direction key up or the direction key down*/
	switch (pLVKeyDow->wVKey)
	{
	case VK_UP:
	{
		if (selectedItemIndex > 0 && selectedItemIndex < m_listCtrlPacketList.GetItemCount())
		{
			m_listCtrlPacketList.SetSelectionMark(--selectedItemIndex );
			selectedItemChangedFlag = true;
		}
	}
	break;
	case VK_DOWN:
	{
		if (selectedItemIndex >= 0 && selectedItemIndex < m_listCtrlPacketList.GetItemCount() - 1)
		{
			m_listCtrlPacketList.SetSelectionMark(++selectedItemIndex);
			selectedItemChangedFlag = true;
		}
	}
	break;
	default:	break;
	}

	/* Select the row to send changes and print packet information and byte stream */
	if (selectedItemChangedFlag)
	{
		CString strPktNum = m_listCtrlPacketList.GetItemText(selectedItemIndex, 0);
		int pktNum = _ttoi(strPktNum);
		if (pktNum < 1 || pktNum > m_pool.getSize())
		{
			return;
		}
		//POSITION pos = g_packetLinkList.FindIndex(pktNum - 1);
		//Packet &pkt = g_packetLinkList.GetAt(pos);
		const Packet &pkt = m_pool.get(pktNum);
		printTreeCtrlPacketDetails(pkt);
		printEditCtrlPacketBytes(pkt);
	}
	
	*pResult = 0;
}


/*************************************************************
*
*		DNS tool function
*
*************************************************************/
/**
*	@brief	Convert domain name name2 with byte count to domain name name1
*			Such as: www.financexyz.com  ->	 www.financexyz.com  
*	@param	name1	domain name
*	@param	name2	Domain name with byte count
*	@return	-
*/
void translateNameInDNS(char *name1, const char *name2)
{
	strcpy(name1, name2);

	char *p = name1;
	bool canMove = false;

	if (!isalnum(*p) && *p != '-')
	{
		canMove = true;
	}

	/* Convert count to '.'*/
	while (*p)
	{
		if (!isalnum(*p) && *p != '-')
		{
			*p = '.';
		}
		++p;
	}

	/* Move the entire domain name forward by 1 position */
	if (canMove)
	{
		p = name1;
		while (*p)
		{
			*p = *(p + 1);
			++p;
		}
	}
}


CString translateNameInDNS(const char *name)
{
	CString strName(name);
	bool canMove = false;

	if (!isalnum(strName.GetAt(0)) && strName.GetAt(0) != '-')
	{
		canMove = true;
	}
	/* Convert count to '.' */
	for (int i = 0; i < strName.GetLength(); ++i)
	{
		if (!isalnum(strName.GetAt(i)) && strName.GetAt(i) != '-')
		{
			strName.SetAt(i, '.');
		}
	}

	/* Move the entire domain name forward by 1 position */
	if (canMove)
	{
		for (int i = 0; i<strName.GetLength(); ++i)
		{
			strName.SetAt(i, strName.GetAt(i + 1));
		}
	}
	return strName;
}
/* Convert the data part of DNS resource record. Convert data2 with pointer 0xc0 to data1 without pointer. The offset is the offset to the DNS header.*/
void translateData(const DNS_Header *dnsh, char *data1, char *data2, const int data2_len)
{
	char *p = data2;
	int count = 0, i = 0;

	/* Traverse data2 */
	while (count < data2_len)
	{
		/* pointer*/
		if (*(u_char*)p == 0xC0)
		{
			++p;

			/* Read the data pointed to by the pointer */
			char *data_ptr = (char*)((u_char*)dnsh + *(u_char*)p);

			int pos = is0xC0PointerInName(data_ptr);
			if (pos)
			{
				translateData(dnsh, data1 + i, data_ptr, pos + 2);
			}
			else
			{
				strcpy(data1 + i, data_ptr);
				i += strlen(data_ptr) + 1;
			}
			count += 2;
		}
		else
		{
			data1[i++] = *p;
			++p;
			++count;
		}
	}

}

/**
*	@brief	Get the name field in DNS (query area, resource record area)
*	@param	name		domain name
*       @param  pDNSHeader      DNS header pointer
*	@param	Domain name string
*/
CString getNameInDNS(char *name, const DNS_Header *pDNSHeader)
{
	int pointerPos;

	// nameÖÐÎÞ0xC0Ö¸Õë
	if ((pointerPos = is0xC0PointerInName(name)) == -1)
	{
		return translateNameInDNS(name);
	}
	else
	{
		int valueOffset = *(name + pointerPos + 1);
		CString value = get0xC0PointerValue(pDNSHeader, valueOffset);

		char *pName = (char*)malloc(pointerPos);
		memcpy(pName, name, pointerPos);
		CString strName(pName);
		strName += value;

		free(pName);
		return strName;

	}
}
/**
*	@brief	Determine whether there is pointer 0xC0 in name, and return the position of the pointer in name
*	@param	name domain name
*	@param	nameLen domain name length
*	@return	Non-0 pointer position in name -1 There is no pointer in name 0xC0 -2 Name is empty
*/
int is0xC0PointerInName(char *name)
{
	if (name == NULL)
	{
		return -2;
	}
	char *p = name;
	int pos = 0;

	while(*p)
	{
		if (*(u_char*)p == 0xC0)
		{
			return pos;
		}
		++p;
		++pos;
	}
	return -1;
}
/**
*	@brief	Get the value of the 0xC0 pointer
*	@param
*	@return 
*/
CString get0xC0PointerValue(const DNS_Header *pDNSHeader, const int offset)
{
	char *pValue = (char*)pDNSHeader + offset;
	CString strValue = getNameInDNS(pValue, pDNSHeader);
	return strValue;
	
}
/*************************************************************
*
*		Menu bar implementation
*
*************************************************************/
/**
*	@brief	(Menu Bar - File - Open) Code Implementation
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnMenuFileOpen()
{
	CFileDialog	dlgFile(TRUE, ".pcap", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("pcapÎÄ¼þ (*.pcap)|*.pcap|ËùÓÐÎÄ¼þ (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal() == IDOK)
	{
		CString openFilePath = dlgFile.GetPathName();
		CString openFileName = dlgFile.GetFileName();
		if (dlgFile.GetFileExt() != "pcap")	// Check file extension
		{
			AfxMessageBox("Unable to open file" + openFileName + "Please check file extension");
			return;
		}
		if (openFileName == m_openFileName)	// Check the file name to avoid opening it repeatedly
		{
			AfxMessageBox("Cannot open the same file repeatedly" + openFileName);
			return;
		}
		if (m_catcher.openAdapter(openFilePath))	
		{
			m_openFileName = openFileName;			// save file name
			AfxGetMainWnd()->SetWindowText(openFileName);	//Modify the title bar to the file name
			m_menu.EnableMenuItem(ID_MENU_FILE_OPEN, MF_ENABLED);	// Enable menu item "Open"
			m_menu.EnableMenuItem(ID_MENU_FILE_CLOSE, MF_ENABLED);	// Enable menu item "Close"
			m_menu.EnableMenuItem(ID_MENU_FILE_SAVEAS, MF_ENABLED);	// Enable menu item "Save As"

			m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_OPEN, TRUE);	// Enable toolbar button "Open"
			m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_SAVEAS, TRUE);	// Enable toolbar button "Save As"

			m_listCtrlPacketList.DeleteAllItems();
			m_treeCtrlPacketDetails.DeleteAllItems();
			m_editCtrlPacketBytes.SetWindowTextA("");
			m_pool.clear();

			m_pktDumper.setPath(openFilePath);
			m_catcher.startCapture(MODE_CAPTURE_OFFLINE);
			m_fileOpenFlag = true;

			CString status = "File opened: " + openFileName;
			updateStatusBar(status, -1, -1);
			//m_statusBar.SetPaneText(0, status, true);		//Modify status bar
		}
	}
}

/**
*	@brief	(Menu Bar - File - Close) Code Implementation
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnMenuFileClose()
{
	if (m_fileOpenFlag)
	{
		AfxGetMainWnd()->SetWindowText("kendoUI");		//Modify title bar
		m_menu.EnableMenuItem(ID_MENU_FILE_OPEN, MF_ENABLED);	// Enable menu item "Open"
		m_menu.EnableMenuItem(ID_MENU_FILE_CLOSE, MF_GRAYED);	// Disable menu item "Close"
		m_menu.EnableMenuItem(ID_MENU_FILE_SAVEAS, MF_GRAYED);	// Disable menu item "Save As"

		m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_OPEN, TRUE);	// Enable toolbar button "Open"
		m_toolBarMain.GetToolBarCtrl().EnableButton(ID_MENU_FILE_SAVEAS, FALSE);// Disable toolbar button "Save As"

		m_listCtrlPacketList.DeleteAllItems();
		m_treeCtrlPacketDetails.DeleteAllItems();
		m_editCtrlPacketBytes.SetWindowTextA("");
		m_pool.clear();

		m_openFileName = "";
		updateStatusBar(CString("ready"), 0, 0);
	}
}

/**
*	@brief	(Menu bar - File - Save as) code implementation
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnMenuFileSaveAs()
{
	CString saveAsFilePath = _T("");
	CString dumpFilePath = m_pktDumper.getPath();
	CString defaultFileName = m_pktDumper.getPath();
	CFileDialog	dlgFile(FALSE, ".pcap", defaultFileName, OFN_OVERWRITEPROMPT, _T("pcap file (*.pcap)|*.pcap|All files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		saveAsFilePath = dlgFile.GetPathName();
		m_pktDumper.dump(saveAsFilePath);
		//m_menu.EnableMenuItem(ID_MENU_FILE_SAVEAS, MF_GRAYED);	// Disable menu item "Save As"
		AfxGetMainWnd()->SetWindowText(dlgFile.GetFileName());		//Modify title bar
		m_statusBar.SetPaneText(0, "Saved to: " + saveAsFilePath, true); //Change status bar	

	}
}
/**
*	@brief	(Menu bar - File - Clean cache files) Code implementation
*	@param	-
*	@return	-
*/

void CkendoUIDlg::OnMenuFileClearCache()
{
	if (clearDirectory(".\\tmp\\"))
	{
		updateStatusBar("Cache files have been cleared", -1, -1);
	}
	else
	{
		updateStatusBar("No cache files to clean", -1, -1);
	}
	
}

/**
*	@brief	(Menu Bar - File - Exit) Code Implementation
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnMenuFileExit()
{
	exit(0);
}

/**
*	@brief	 (Menu Bar - Help - About) Code Implementation
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnMenuHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

/**
*	@brief	(Menu bar - Help - List of shortcut keys) Code implementation
*	@param	-
*	@return	-
*/
void CkendoUIDlg::OnMenuHelpShortCut()
{
	CShortCutDialog dlg;
	dlg.DoModal();
}
/*************************************************************
*
*		Shortcut key implementation
*
*************************************************************/
BOOL CkendoUIDlg::PreTranslateMessage(MSG * pMsg)
{
	if (::TranslateAccelerator(m_hWnd, m_hAccelMenu, pMsg))
	{
		return(TRUE);
	}
	else
	{
		if (GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == 'G')
			OnAcceleratorCtrlG();
	}
	return   CDialog::PreTranslateMessage(pMsg);
}
