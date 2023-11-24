#include "stdafx.h"
#include "PacketPool.h"

PacketPool::PacketPool()
{
}


PacketPool::~PacketPool()
{
}

/**
*	@brief	Add packet to pool
*	@param	pkt_data	data pack	
*	@param	header		capital
*	@return	-
*/
void PacketPool::add(const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	if (header && pkt_data)
	{
		int pktNum = 1 + m_pkts.size();
		Packet pkt(header, pkt_data, pktNum);
		m_pkts[pktNum] = pkt;
	}
}

/**
*	@brief	Add packet to pool
*	@param	pkt	data pack
*	@return	-
*/
void PacketPool::add(const Packet &pkt)
{
	if(!pkt.isEmpty())
		m_pkts[pkt.num] = pkt;
}

/**
*	@brief	Delete the specified packet from the pool based on the packet number
*	@param	pktNum	Packet number
*	@return	-
*/
void PacketPool::remove(int pktNum)
{
	if (pktNum < 1 || pktNum > m_pkts.size())
		return;
	m_pkts.erase(pktNum);
}

void PacketPool::clear()
{
	if (m_pkts.size() > 0)
		m_pkts.clear();
}

/**
*	@brief	Get the specified data packet from the pool based on the data packet number
*	@param	pktNum	Packet number
*	@return	pkt		Packet reference
*/
Packet& PacketPool::get(int pktNum)
{
	if (m_pkts.count(pktNum) > 0)
		return m_pkts[pktNum];
	return Packet();
}

/**
*	@brief	Get the last packet from the pool
*	@param	pktNum	Packet number
*	@return	pkt		Packet reference
*/
Packet& PacketPool::getLast()
{	
	if (m_pkts.count(m_pkts.size()) > 0)
		return m_pkts[m_pkts.size()];
	return Packet();
}

/**
*	@brief	Get the number of data packets in the pool
*	@param	-
*	@return	Number of data packets
*/
int PacketPool::getSize() const
{
	return m_pkts.size();
}

/**
*	@brief	Determine whether the pool is empty
*	@param	-
*	@return	true empty false not empty
*/
bool PacketPool::isEmpty() const
{
	if (m_pkts.size()) 
		return false;
	return true;
}


