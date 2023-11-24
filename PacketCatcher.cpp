#include "stdafx.h"
#include "PacketCatcher.h"
#include "Packet.h"
#include "ThreadParam.h"
#define HAVE_REMOTE
#include "pcap.h"

PacketCatcher::PacketCatcher()
{
	m_adhandle = NULL;
	m_pool = NULL;
	m_dumper = NULL;
	m_devlist = NULL;
}

PacketCatcher::PacketCatcher(PacketPool * pool)
{
	m_adhandle = NULL;
	m_pool = pool;
	m_dumper = NULL;
	m_devlist = NULL;
}


PacketCatcher::~PacketCatcher()
{
	m_adhandle = NULL;
	m_pool = NULL;
	m_dumper = NULL;
	if (m_devlist)
	{
		pcap_freealldevs(m_devlist); 
		m_devlist = NULL;
	}
		
}

bool PacketCatcher::setPool(PacketPool *pool)
{
	if (pool)
	{
		m_pool = pool;
		return true;
	}
	return false;
}

/**
*	@brief	Open the specified network card and obtain the network card descriptor adhandle
*	@param	selItemIndexOfDevList	The subscript of the selected item in the network card list
*	@return true Open successfully	false	fail
*/
bool PacketCatcher::openAdapter(int selItemIndexOfDevList, const CTime &currentTime)
{
	if (selItemIndexOfDevList < 0 || m_adhandle)
		return false;

	int count = 0, selDevIndex = selItemIndexOfDevList - 1;
	//pcap_if_t *dev, *allDevs;
	//char errbuf[PCAP_ERRBUF_SIZE + 1];
	//if (pcap_findalldevs(&allDevs, errbuf) == -1)
	//{
	//	AfxMessageBox(_T("pcap_findalldevsmistake!"), MB_OK);
	//	return false;
	//}
	//for (dev = allDevs; count < selDevIndex; dev = dev->next, ++count);
	pcap_if_t* dev;
	for (dev = m_devlist; count < selDevIndex; dev = dev->next, ++count);
	// Network card information copy
	m_dev = dev->description + CString(" ( ") + dev->name + " )";

	// Open network card
	if ((m_adhandle = pcap_open_live(dev->name,
		65535,						// maximum capture length
		PCAP_OPENFLAG_PROMISCUOUS,	// Set the network card to promiscuous mode
		READ_PACKET_TIMEOUT,		// Read timeout
		NULL)) == NULL)
	{
		AfxMessageBox(_T("pcap_open_livemistake!"), MB_OK);
		return false;
	}

	/* Open dump file */
	CString file = "kendoUI_" + currentTime.Format("%Y%m%d%H%M%S") + ".pcap";
	CString path = ".\\tmp\\" + file;
	m_dumper = pcap_dump_open(m_adhandle, path);

	//pcap_freealldevs(allDevs);
	return true;
}

/**
*	@brief	Open the specified file and obtain the file descriptor adhandle
*	@param	path	file path
*	@return true	Open successfully	false	fail
*/
bool PacketCatcher::openAdapter(CString path)
{
	if (path.IsEmpty())
		return false;
	m_dev = path;
	if ( (m_adhandle = pcap_open_offline(path, NULL))  == NULL)
	{
		AfxMessageBox(_T("pcap_open_offlinemistake!"), MB_OK);
		return false;
	}
	return true;
}

/**
*	@brief	Close an open network card
*	@param	-
*	@return true	Close successfully false Close failed
*/
bool PacketCatcher::closeAdapter()
{
	if (m_adhandle)
	{
		pcap_close(m_adhandle);
		m_adhandle = NULL;
		if (m_dumper)
		{
			pcap_dump_close(m_dumper);
			m_dumper = NULL;
		}
		return true;
	}
	return false;
}

/**
*	@brief	Start capturing packets
*	@param	-
*	@return -
*/
void PacketCatcher::startCapture(int mode)
{
	if (m_adhandle && m_pool)
		AfxBeginThread(capture_thread, new ThreadParam(m_adhandle, m_pool, m_dumper, mode));
}

/**
*	@brief	ͣ  Stop capturing packets
*	@param	-
*	@return -
*/
void PacketCatcher::stopCapture()
{
	if (m_adhandle)
		pcap_breakloop(m_adhandle);
}

CString PacketCatcher::getDevName()
{
	return m_dev;
}

void PacketCatcher::setDevList(pcap_if_t *devlist)
{
	m_devlist = devlist;
}


/**
*	@brief Capture packet thread entry function, global function
*	@param pParam Parameters passed to the thread
*	@return 0 Indicates that the packet capture was successful -1 indicates that the packet capture failed
*/
UINT capture_thread(LPVOID pParam)
{
	ThreadParam *p = (ThreadParam*)pParam;

	/* Start capturing packets */
	pcap_loop(p->m_adhandle, -1, packet_handler, (unsigned char *)p);
	PostMessage(AfxGetMainWnd()->m_hWnd, WM_TEXIT, NULL, NULL);
	return 0;
}

/**
*	@brief	Capture packet processing function, global callback function
*	@param	param		Custom parameters
*	@param	header		Packet header
*	@param	pkt_data	Packet (frame)
*	@return
*/
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	ThreadParam *threadParam = (ThreadParam *)param;
	// Capture packets according to capture mode
	switch (threadParam->m_mode)
	{
	case MODE_CAPTURE_LIVE:
	{		
		threadParam->m_pool->add(header, pkt_data);
		pcap_dump((u_char*)threadParam->m_dumper, header, pkt_data);
		break;
	}
	case MODE_CAPTURE_OFFLINE:
	{
		threadParam->m_pool->add(header, pkt_data);
		break;
	}
	}

	// Send message to main window kendoUIDlg
	PostMessage(AfxGetMainWnd()->m_hWnd, WM_PKTCATCH, NULL, (LPARAM)(threadParam->m_pool->getLast().num));

	// If the packet is captured online, let the thread sleep for 0.5 seconds to prevent the interface from freezing.
	if (threadParam->m_mode == MODE_CAPTURE_LIVE) {
		Sleep(500);
	}
}
