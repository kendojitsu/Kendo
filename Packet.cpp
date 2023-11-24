#include "stdafx.h"
#include "Packet.h"
#include "pcap.h"
Packet::Packet()
{
	ethh = NULL;
	iph = NULL;
	arph = NULL;
	icmph = NULL;
	udph = NULL;
	tcph = NULL;
	dnsh = NULL;
	dhcph = NULL;
	httpmsg = NULL;

	pkt_data = NULL;
	num = -1;
	header = NULL;
}

Packet::Packet(const Packet &p)
{
	ethh = NULL;
	iph = NULL;
	arph = NULL;
	icmph = NULL;
	udph = NULL;
	tcph = NULL;
	dnsh = NULL;
	dhcph = NULL;
	httpmsg = NULL;

	if (!p.isEmpty())
	{
		u_int caplen = p.header->caplen;

		pkt_data = (u_char*)malloc( caplen );
		memcpy(pkt_data, p.pkt_data, caplen);

		header = (struct pcap_pkthdr *)malloc(sizeof( *(p.header) ));
		memcpy(header, p.header, sizeof(*(p.header)));
		
		num = p.num;

		decodeEthernet();
	}
	else
	{
		pkt_data = NULL;
		header = NULL;
		num = -1;
	}
}

Packet::Packet(const struct pcap_pkthdr *header,const u_char *pkt_data, const u_short &packetNum)
{
	ethh = NULL;
	iph = NULL;
	arph = NULL;
	icmph = NULL;
	udph = NULL;
	tcph = NULL;
	dnsh = NULL;
	dhcph = NULL;
	httpmsg = NULL;
	num = packetNum;

	if (pkt_data != NULL && header != NULL)
	{
		this->pkt_data = (u_char*)malloc(header->caplen);
		memcpy(this->pkt_data, pkt_data, header->caplen);

		this->header = (struct pcap_pkthdr *)malloc(sizeof(*header));
		memcpy(this->header, header, sizeof(*header));

		decodeEthernet();
	}
	else
	{
		this->pkt_data = NULL;
		this->header = NULL;
	}
}

/**
*	@brief	assignment operator function
*	@param	p	data pack
*	@return instance itself
*/
Packet & Packet::operator=(const Packet & p)
{
	if (this == &p)
	{
		return *this;
	}
	ethh = NULL;
	iph = NULL;
	arph = NULL;
	icmph = NULL;
	udph = NULL;
	tcph = NULL;
	dnsh = NULL;
	dhcph = NULL;


	if (!p.isEmpty())
	{
		u_int caplen = p.header->caplen;

		if (pkt_data == NULL)
		{
			pkt_data = (u_char*)malloc(caplen);
		}
		memcpy(pkt_data, p.pkt_data, caplen);

		if (header == NULL)
		{
			header = (struct pcap_pkthdr *)malloc(sizeof(*(p.header)));
		}
		memcpy(header, p.header, sizeof(*(p.header)));

		num = p.num;

		decodeEthernet();
	}
	else
	{
		pkt_data = NULL;
		header = NULL;
		httpmsg = NULL;
		num = -1;
	}
	return *this;
}

Packet::~Packet()
{
	ethh = NULL;
	iph = NULL;
	arph = NULL;
	icmph = NULL;
	udph = NULL;
	tcph = NULL;
	dnsh = NULL;
	dhcph = NULL;
	httpmsg = NULL;
	num = -1;

	free(pkt_data);
	pkt_data = NULL;

	free(header);
	header = NULL;
	protocol.Empty();
}

/**
*	@brief	Determine whether the data packet is empty
*	@param	-
*	@return true pkt_data or header is empty false pkt_data and header are both
*/
bool Packet::isEmpty() const
{
	if (pkt_data == NULL || header == NULL)
	{
		return true;
	}
	return false;
}

/**
*	@brief	Parse the Ethernet frame, save it with the member variable ethh, and call the next parser based on the eth_type value
*	@param	-
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/

int Packet::decodeEthernet()
{
	if (isEmpty())
	{
		return -1;
	}

	protocol = "Ethernet";
	ethh = (Ethernet_Header*)pkt_data;
	
	switch (ntohs(ethh->eth_type))
	{
		case ETHERNET_TYPE_IP:
			decodeIP(pkt_data + ETHERNET_HEADER_LENGTH);
			break;
		case ETHERNET_TYPE_ARP:
			decodeARP(pkt_data + ETHERNET_HEADER_LENGTH);
			break;
		default:
			break;
	}
	return 0;
}

/**
*	@brief	Parse the IP packet header, save it with the member variable iph, and call the next parser based on the protocol value
*	@param	L2payload	Pointer to IP packet
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeIP(u_char * L2payload)
{
	if (L2payload == NULL)
	{
		return -1;
	}

	protocol = "IPv4";
	iph = (IP_Header*)(L2payload);
	u_short IPHeaderLen = (iph->ver_headerlen & 0x0f) * 4;
	switch (iph->protocol)
	{
		case PROTOCOL_ICMP:		
			decodeICMP(L2payload + IPHeaderLen);
			break;	

		case PROTOCOL_TCP:		
			decodeTCP(L2payload + IPHeaderLen);
			break;	

		case PROTOCOL_UDP:
			decodeUDP(L2payload + IPHeaderLen);
			break;	

		default:
			break;
	}
	return 0;
}

/**
*	@brief	Parse the ARP message header and save it with the member variable arph
*	@param	L2payload	Pointer to ARP packet
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeARP(u_char * L2payload)
{
	if (L2payload == NULL)
	{
		return -1;
	}
	protocol = "ARP";
	arph = (ARP_Header*)(L2payload);

	return 0;
}

/**
*	@brief	Parse the ICMP message header and save it with the member variable icmph
*	@param	L2payload	Pointer to ICMP message
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeICMP(u_char * L3payload)
{
	if (L3payload == NULL)
	{
		return -1;
	}

	protocol = "ICMP";
	icmph = (ICMP_Header*)(L3payload);
	return 0;
}

/**
*	@brief	Parse the TCP segment header, save it with the member variable tcph, and select the next parser based on the source and destination ports.
*	@param	L3payload	Pointer to TCP segment
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeTCP(u_char * L3payload)
{
	if (L3payload == NULL)
	{
		return -1;
	}

	protocol = "TCP";
	tcph = (TCP_Header*)(L3payload);

	u_short TCPHeaderLen = (ntohs(tcph->headerlen_rsv_flags) >> 12) * 4;
	if (ntohs(tcph->srcport) == PORT_DNS || ntohs(tcph->dstport) == PORT_DNS)
	{
		decodeDNS(L3payload + TCPHeaderLen);
	}
	else if (ntohs(tcph->srcport) == PORT_HTTP || ntohs(tcph->dstport) == PORT_HTTP)
	{
		int HTTPMsgLen = getL4PayloadLength();
		if (HTTPMsgLen > 0)
		{
			decodeHTTP(L3payload + TCPHeaderLen);
		}
		
	}
	return 0;
}

/**
*	@brief	Parse the UDP user datagram header, save it with the member variable udph, and select the next parser based on the source and destination ports.
*	@param	L2payload	Pointer to UDP user datagram
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeUDP(u_char *L3payload)
{
	if (L3payload == NULL)
	{
		return -1;
	}

	protocol = "UDP";
	udph = (UDP_Header*)(L3payload);
	if (ntohs(udph->srcport) == PORT_DNS || ntohs(udph->dstport) == PORT_DNS)
	{
		decodeDNS(L3payload + UDP_HEADER_LENGTH);

	}
	else if ((ntohs(udph->srcport) == PORT_DHCP_CLIENT && ntohs(udph->dstport) == PORT_DHCP_SERVER) || (ntohs(udph->srcport) == PORT_DHCP_SERVER && ntohs(udph->dstport) == PORT_DHCP_CLIENT))
	{
		decodeDHCP(L3payload + UDP_HEADER_LENGTH);
	}
	return 0;
}

/**
*	@brief	Parse the DNS message header and save it with the member variable dnsh
*	@param	L4payload	ָPointer to DNS message
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeDNS(u_char * L4payload)
{
	if (L4payload == NULL)
	{
		return -1;
	}

	protocol = "DNS";
	dnsh = (DNS_Header*)(L4payload);
	return 0;
}

/**
*	@brief	Parse the DHCP message header and save it with the member variable dhcph
*	@param	L4payload	Pointer to DHCP message
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeDHCP(u_char * L4payload)
{
	if (L4payload == NULL)
	{
		return -1;
	}

	protocol = "DHCP";
	dhcph = (DHCP_Header*)L4payload;
	return 0;
}

/**
*	@brief	Parse the HTTP message header and save it with the member variable httpmsg
*	@param	L4payload	Pointer to httpmsg message
*	@return	0 Indicates parsing success -1 indicates parsing failure
*/
int Packet::decodeHTTP(u_char * L4payload)
{
	if (L4payload == NULL)
	{
		return -1;
	}

	protocol = "HTTP";
	httpmsg = L4payload;
	return 0;
}

/**
*	@brief	Get IP header length
*	@param	-
*	@return IP header length
*/
int Packet::getIPHeaderLegnth() const
{
	if (iph == NULL)
		return -1;
	else
		return (iph->ver_headerlen & 0x0F) * 4;
}

/**
*	@brief	Get the original value of the IP header length
*	@param	-
*	@return Original value of IP header length -1 IP header is empty
*/
int Packet::getIPHeaderLengthRaw() const
{
	if (iph == NULL)
		return -1;
	else
		return (iph->ver_headerlen & 0x0F);
}

/**
*	@brief	Get IP header flag
*	@param	-
*	@return IP header flag -1 IP header is empty
*/
int Packet::getIPFlags() const
{
	if (iph == NULL)
		return -1;
	else
		return ntohs(iph->flags_offset) >> 13;
}

/**
*	@brief	Get the IP header flag DF bit
*	@param	-
*	@return IP header flag DF bit -1 IP header is empty
*/
int Packet::getIPFlagDF() const
{
	if (iph == NULL)
		return -1;
	else
		return (ntohs(iph->flags_offset) >> 13) & 0x0001;
}

/**
*	@brief	Get the IP header flag MF bit
*	@param	-
*	@return IP header flag MF bit -1 IP header is empty
*/
int Packet::getIPFlagsMF() const
{
	if (iph == NULL)
		return -1;
	else
		return (ntohs(iph->flags_offset) >> 14) & 0x0001;
}

/**
*	@brief	Get IP header slice offset
*	@param	-
*	@return IP header piece offset -1 IP header is empty
*/
int Packet::getIPOffset() const
{
	if (iph == NULL)
		return -1;
	else
		return	ntohs(iph->flags_offset) & 0x1FFF;
}

/**
*	@brief	Get the Id in the Other field of the ICMP header
*	@param	-
*	@return Id -1 in the Other field of the ICMP header The ICMP header is empty
*/
u_short Packet::getICMPID() const
{
	if (icmph == NULL)
		return -1;
	else
	return (u_short)(ntohl(icmph->others) >> 16);
}

/**
*	@brief	Get the Seq in the Other field of the ICMP header
*	@param	-
*	@return Seq -1 in the Other field of the ICMP header is empty.
*/
u_short Packet::getICMPSeq() const
{
	if (icmph == NULL)
		return -1;
	else
		return (u_short)(ntohl(icmph->others) & 0x0000FFFF);
}

/**
*	@brief	Get TCP header length
*	@param	-
*	@return TCP header length -1 TCP header is empty
*/
int Packet::getTCPHeaderLength() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 12) * 4;
}

/**
*	@brief	Get the original value of TCP header length
*	@param	-
*	@return Original value of TCP header length -1 TCP header is empty
*/
int Packet::getTCPHeaderLengthRaw() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 12) ;
}

/**
*	@brief	Get TCP header flags
*	@param	-
*	@return TCP header flag -1 TCP header
*/
u_short Packet::getTCPFlags() const
{
	if (tcph == NULL)
		return -1;
	else
		return  ntohs(tcph->headerlen_rsv_flags) & 0x0FFF;
}

/**
*	@brief	Get TCP header flags
*	@param	-
*	@return TCP header flag -1 TCP header is empty
*/
int Packet::getTCPFlagsURG() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 5) & 0x0001;
}

/**
*	@brief	Get TCP header flag ACK
*	@param	-
*	@return TCP header flag ACK -1 TCP header is empty
*/
int Packet::getTCPFlagsACK() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 4) & 0x0001;
}

/**
*	@brief	Get the TCP header flag PSH
*	@param	-
*	@return TCP header flag PSH -1 TCP header is empty
*/
int Packet::getTCPFlagsPSH() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 3) & 0x0001;
}

/**
*	@brief	Get TCP header flag RST
*	@param	-
*	@return TCP header flag RST -1 TCP header is empty
*/
int Packet::getTCPFlagsRST() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 2) & 0x0001;
}

/**
*	@brief	Get the TCP header flag SYN
*	@param	-
*	@return TCP header flag SYN -1 TCP header is empty
*/
int Packet::getTCPFlagsSYN() const
{
	if (tcph == NULL)
		return -1;
	else
		return (ntohs(tcph->headerlen_rsv_flags) >> 1) & 0x0001;
}

/**
*	@brief	Get the TCP header flag FIN
*	@param	-
*	@return TCP header flag FIN -1 TCP header is empty
*/
int Packet::getTCPFlagsFIN() const
{
	if (tcph == NULL)
		return -1;
	else
		return ntohs(tcph->headerlen_rsv_flags) & 0x0001;
}
/**
*	@brief Get the TCP header flag FIN
*	@param	-
*	@return TCP header flag FIN -1 TCP header is empty
*/
int Packet::getL4PayloadLength() const
{
	if (iph == NULL || tcph == NULL)
	{
		return 0;
	}
	int IPTotalLen = ntohs(iph->totallen);
	int IPHeaderLen = (iph->ver_headerlen & 0x0F) * 4;
	int TCPHeaderLen = (ntohs(tcph->headerlen_rsv_flags) >> 12 ) * 4;

	return IPTotalLen - IPHeaderLen - TCPHeaderLen ;
}

/**
*	@brief	Get the DNS header flag QR
*	@param	-
*	@return DNS header flag QR -1 DNS header is empty
*/

int Packet::getDNSFlagsQR() const
{
	if (dnsh == NULL)
		return -1;
	else
		return	dnsh->flags >> 15;
}

/**
*	@brief	Get the DNS header flag OPCODE
*	@param	-
*	@return DNS header flag OPCODE -1 DNS header is empty
*/
int Packet::getDNSFlagsOPCODE() const
{
	if (dnsh == NULL)
		return -1;
	else
		return	(ntohs(dnsh->flags) >> 11) & 0x000F;
}

/**
*	@brief	Get DNS header flag AA
*	@param	-
*	@return DNS header flag AA -1 DNS header is empty
*/
int Packet::getDNSFlagsAA() const
{
	if (dnsh == NULL)
		return -1;
	else
		return (ntohs(dnsh->flags) >> 10) & 0x0001;
}

/**
*	@brief	Get the DNS header flag TC
*	@param	-
*	@return DNS header flag TC -1 DNS header is empty
*/
int Packet::getDNSFlagsTC() const
{
	if (dnsh == NULL)
		return -1;
	else
		return (ntohs(dnsh->flags) >> 9) & 0x0001;
}

/**
*	@brief	Get the DNS header flag RD
*	@param	-
*	@return DNS header flag RD -1 DNS header is empty
*/
int Packet::getDNSFlagsRD() const
{
	if (dnsh == NULL)
		return -1;
	else
		return (ntohs(dnsh->flags) >> 8) & 0x0001;
}

/**
*	@brief	Get the DNS header flag RA
*	@param	-
*	@return DNS header flag RA -1 DNS header is empty
*/
int Packet::getDNSFlagsRA() const
{
	if (dnsh == NULL)
		return -1;
	else
		return (ntohs(dnsh->flags) >> 7) & 0x0001;
}

/**
*	@brief	Get DNS header flag Z
*	@param	-
*	@return DNS header flag Z -1 DNS header is empty
*/
int Packet::getDNSFlagsZ() const
{
	if (dnsh == NULL)
		return -1;
	else
		return (ntohs(dnsh->flags) >> 4) & 0x0007;
}

/**
*	@brief	Get the DNS header flag RCODE
*	@param	-
*	@return DNS header flag RCODE -1 DNS header is empty
*/
int Packet::getDNSFlagsRCODE() const
{
	if (dnsh == NULL)
		return -1;
	else
		return ntohs(dnsh->flags) & 0x000F;
}
