#pragma once
#include <map>
#include "Packet.h"

/* This type of management data package */
class PacketPool
{
private:
	std::map<int, Packet> m_pkts;		// Store data packets, key is the data packet number, value is the data packet

public:
	PacketPool();
	~PacketPool();

	void add(const struct pcap_pkthdr *header, const u_char *pkt_data);
	void add(const Packet &pkt);
	void remove(int pktNum);
	void clear();
	Packet& get(int pktNum);
	Packet& getLast();
	int getSize() const;
	bool isEmpty() const;
};

