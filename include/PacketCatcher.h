#pragma once
#include "PacketPool.h"
#include "pcap.h"

#define WM_PKTCATCH	(WM_USER + 100)
#define WM_TEXIT	(WM_USER + 101)

const int MODE_CAPTURE_LIVE = 0;
const int MODE_CAPTURE_OFFLINE = 1;
const int READ_PACKET_TIMEOUT = 1000;

UINT capture_thread(LPVOID pParam);
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

/* This class is used to capture data packets and store them in packet pools and files. */
class PacketCatcher
{
private:
	pcap_t			*m_adhandle;	// network card descriptor
	PacketPool		*m_pool;		// Pointer to packet pool
	pcap_dumper_t	*m_dumper;		// Dump file descriptor
	CString         m_dev;			// Network card/file information
	pcap_if_t		*m_devlist;		// Network card list

public:
	PacketCatcher();
	PacketCatcher(PacketPool *pool);
	~PacketCatcher();

	bool setPool(PacketPool *pool);
	bool openAdapter(int selItemIndexOfDevList, const CTime &currentTime);
	bool openAdapter(CString path);
	bool closeAdapter();
	void startCapture(int mode);
	void stopCapture();
	CString getDevName();
	void setDevList(pcap_if_t* devlist);
};

