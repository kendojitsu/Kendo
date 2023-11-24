#include "pcap.h"
//#include "remote-ext.h"
#include "Afxtempl.h"
#include "Packet.h"

#define PCAP_ERRBUFF_SIZE	50

/* Heap file name */
//char filename[50];

/* control pointer */
//CButton			*g_pBtnStart;
//CButton			*g_pBtnPause;
//CButton			*g_pBtnStop;
//CButton			*g_pBtnFilter;
//CButton			*g_pBtnClear;
//CComboBox		*g_pComboBoxDevList;
//CListCtrl		*g_pListCtrlPacketList;
//CTreeCtrl		*g_pTreeCtrlPacketInfo;
//CEdit			*g_pEditCtrlPacketData;
////CRichEditCtrl	*g_pRichEditCtrlFilterInput;
//CComboBox		*g_pComboBoxlFilterInput;

///* Network card information */
//pcap_if_t *g_pAllDevs,*g_pDev;
//
///* descriptor of the open capture instance in pcap */
//pcap_t *g_pAdhandle;
//
///* heap files */
//pcap_dumper_t *g_dumpfile;
//CString g_dumpFileName;

/* The global variable errbuf stores error information */
//char g_errbuf[PCAP_ERRBUF_SIZE];



/* Packet list row, number */
//int g_listctrlPacketListRows = -1;
//int g_listctrlPacketListCols = 0;
//int g_listctrlPacketListCount = 0;
//
//u_short g_packetCaptureSum = 0;

/* Linked list, storing Packet class instances */
//CList<Packet, Packet> g_packetLinkList;

/* Thread entry function */
//UINT capture_thread(LPVOID pParam);

/* Capture handler function */
//void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

/* Control initialization */
//void initialComboBoxDevList();
//void initialListCtrlPacketList();
//void initialComboBoxFilterList();

/* Print */
//int printListCtrlPacketList(const Packet &pkt);
//int printListCtrlPacketList(const PacketPool &pool);
//int printListCtrlPacketList(const PacketPool &pool, const CString &filter);
//
//
//int	printEditCtrlPacketData(const Packet &pkt);
//
//int printTreeCtrlPacketInfo(const Packet &pkt);
//int printEthernet2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printIP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printARP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printICMP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printTCP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printUDP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printDNS2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printDHCP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);
//int	printHTTP2TreeCtrl(const Packet &pkt, HTREEITEM &parentNode);

/* Convert */
//CString	MACAddr2CString(const MAC_Address &addr);
//CString	IPAddr2CString(const IP_Address &addr);

/* Domain name conversion: Convert name2 in the specified format to domain name name1 */
void translateNameInDNS(char *name1, const char *name2);

/*Convert the data part of the DNS resource record. Convert the address data2 with the pointer c0 to the address data1. The offset is the offset to the dns header.*/
void translateData(const DNS_Header *dnsh, char *data1, char *data2, const int data2_len);

/*Determine whether there is pointer 0xc0 in data, and return the position of the pointer in data*/
int is0xC0PointerInName(char *name);

CString getNameInDNS(char *name, const DNS_Header *pDNSHeader);
CString get0xC0PointerValue(const DNS_Header *pDNSHeader, const int offset);
int is0xC0PointerInName(char *name);