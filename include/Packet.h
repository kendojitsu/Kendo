#pragma once
#include "ProtocolHeader.h"

class Packet
{
public:
	Ethernet_Header			*ethh;			// ��̫���ײ�
	IP_Header				*iph;			// IP�ײ�
	ARP_Header				*arph;			// ARP�ײ�
	ICMP_Header				*icmph;			// ICMP�ײ�
	TCP_Header				*tcph;			// TCP�ײ�
	UDP_Header				*udph;			// UDP�ײ�
	DNS_Header				*dnsh;			// DNS�ײ�
	DHCP_Header				*dhcph;			// DHCP�ײ�
	u_char					*httpmsg;		// HTTP����

	u_char					*pkt_data;		// ���ݰ���֡��
	struct pcap_pkthdr		*header;		// �������ݰ����ȣ����ݰ����ȣ����ݰ�����ʱ��
	u_short					num;			// ���ݰ���ţ���1��ʼ
	CString					protocol;		// Э��


	Packet();
	Packet(const Packet &p);
	Packet(const struct pcap_pkthdr *header, const u_char *pkt_data, const u_short &packetNum);
	Packet& operator=(const Packet	&p);
	~Packet();

	bool isEmpty() const;

	int decodeEthernet();
	int decodeIP(u_char *L2payload);
	int decodeARP(u_char *L2payload);
	int decodeICMP(u_char *L3payload);
	int decodeTCP(u_char *L3payload);
	int decodeUDP(u_char *L3payload);
	int decodeDNS(u_char *L4payload);
	int decodeDHCP(u_char *L4payload);
	int decodeHTTP(u_char *L4payload);

	int getIPHeaderLegnth() const;
	int getIPHeaderLengthRaw() const;
	int getIPFlags() const;
	int getIPFlagsMF() const;
	int getIPFlagDF() const;
	int getIPOffset() const;

	u_short getICMPID()	const;
	u_short getICMPSeq() const;

	int getTCPHeaderLength() const;
	int getTCPHeaderLengthRaw() const;
	u_short getTCPFlags()	const;
	int getTCPFlagsURG()	const;
	int getTCPFlagsACK()	const;
	int getTCPFlagsPSH()	const;
	int getTCPFlagsRST()	const;
	int getTCPFlagsSYN()	const;
	int getTCPFlagsFIN()	const;

	int getL4PayloadLength() const;

	int getDNSFlagsQR()		const;
	int getDNSFlagsOPCODE()	const;
	int getDNSFlagsAA()		const;
	int getDNSFlagsTC()		const;
	int getDNSFlagsRD()		const;
	int getDNSFlagsRA()		const;
	int getDNSFlagsZ()		const;
	int getDNSFlagsRCODE()	const;
};