#pragma once

#define ETHERNET_HEADER_LENGTH	14
#define	UDP_HEADER_LENGTH		8
#define DNS_HEADER_LENGTH		12

#define	ETHERNET_TYPE_IP		0x0800
#define	ETHERNET_TYPE_ARP		0x0806

#define PROTOCOL_ICMP			1
#define PROTOCOL_TCP			6
#define PROTOCOL_UDP			17

#define PORT_DNS				53
#define	PORT_DHCP_CLIENT		67
#define PORT_DHCP_SERVER		68
#define PORT_HTTP				80

#define ARP_OPCODE_REQUET		1
#define	ARP_OPCODE_REPLY		2

/**
*	@brief	ICMP Type
*/
#define ICMP_TYPE_ECHO_REPLY													0
#define	ICMP_TYPE_DESTINATION_UNREACHABLE										3
#define ICMP_TYPE_SOURCE_QUENCH													4
#define ICMP_TYPE_REDIRECT														5
#define ICMP_TYPE_ECHO															8
#define ICMP_TYPE_ROUTER_ADVERTISEMENT											9
#define ICMP_TYPE_ROUTER_SOLICITATION											10
#define ICMP_TYPE_TIME_EXCEEDED													11
#define ICMP_TYPE_PARAMETER_PROBLEM												12
#define ICMP_TYPE_TIMESTAMP														13
#define ICMP_TYPE_TIMESTAMP_REPLY												14

/**
*	@brief	ICMP Code
*/
#define ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_NET_UNREACHABLE					0
#define ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_HOST_UNREACHABLE					1
#define ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_PROTOCOL_UNREACHABLE				2
#define ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_PORT_UNREACHABLE					3
#define ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_FRAGMENTATION_NEEDED_AND_DF_SET	4
#define ICMP_TYPE_DESTINATION_UNREACHABLE_CODE_SOURCE_ROUTE_FAILED				5

#define ICMP_TYPE_SOURCE_QUENCH_CODE											0

#define ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_NETWORK				0
#define ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_HOST					1
#define ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_TOS_AND_NETWORK		2
#define ICMP_TYPE_REDIRECT_CODE_REDIRECT_DATAGRAMS_FOR_THE_TOS_AND_HOST			3

#define ICMP_TYPE_ECHO_CODE														0

#define ICMP_TYPE_ROUTER_ADVERTISEMENT_CODE										0
#define ICMP_TYPE_ROUTER_SOLICITATION_CODE										0

#define ICMP_TYPE_TIME_EXCEEDED_CODE_TTL_EXCEEDED_IN_TRANSIT					0
#define ICMP_TYPE_TIME_EXCEEDED_CODE_FRAGMENT_REASSEMBLY_TIME_EXCEEDE			1

#define ICMP_TYPE_PARAMETER_PROBLEM_CODE_POINTER_INDICATES_THE_ERROR			0	

#define ICMP_TYPE_TIMESTAMP_CODE												0

/**
*	@brief DNS Flags
*/
#define DNS_FLAGS_QR_REQUEST					0
#define DNS_FLAGS_QR_REPLY						1

#define DNS_FLAGS_OPCODE_STANDARD_QUERY			0
#define DNS_FLAGS_OPCODE_INVERSE_QUERY			1
#define DNS_FLAGS_OPCODE_SERVER_STATUS_REQUEST	2

#define DNS_FLAGS_RCODE_NO_ERROR				0
#define DNS_FLAGS_RCODE_FORMAT_ERROR			1
#define DNS_FLAGS_RCODE_SERVER_FAILURE			2
#define DNS_FLAGS_RCODE_NAME_ERROR				3
#define DNS_FLAGS_RCODE_NOT_IMPLEMENTED			4
#define DNS_FLAGS_RCODE_REFUSED					5
/**
*	@brief	DNS Type in Resource Record
*			see more in RFC1035
*/
#define DNS_TYPE_A		1	// implemented
#define DNS_TYPE_NS		2	// implemented
#define DNS_TYPE_MD		3
#define DNS_TYPE_MF		4
#define DNS_TYPE_CNAME	5	// implemented
#define DNS_TYPE_SOA	6	// implemented
#define DNS_TYPE_MB		7
#define DNS_TYPE_MG		8
#define DNS_TYPE_MR		9
#define DNS_TYPE_NULL	10
#define DNS_TYPE_WKS	11
#define DNS_TYPE_PTR	12	// implemented
#define DNS_TYPE_HINFO	13
#define DNS_TYPE_MINFO	14
#define DNS_TYPE_MX		15
#define DNS_TYPE_TXT	16
#define DNS_TYPE_AAAA	28	// implemented
#define DNS_TYPE_ANY	255	// implemented

/**
*	@brief	DNS Class in Resource Record
*			see more in RFC1035
*/
#define DNS_CLASS_IN	1
#define DNS_CLASS_CS	2
#define DNS_CLASS_CH	3
#define DNS_CLASS_HS	4

/**
*	@brief	DNS Resource Record Type
*/
#define DNS_RESOURCE_RECORD_TYPE_ANSWER			0
#define DNS_RESOURCE_RECORD_TYPE_AUTHORITY		1
#define DNS_RESOURCE_RECORD_TYPE_ADDITIONAL		2

/** 
*	@brief DHCP Flags and Options
*/
#define	DHCP_FLAGS_BROADCAST	0
#define DHCP_FLAGS_UNICAST		1

/* RFC 1497 Vendor Extensions */
#define DHCP_OPTIONS_PAD_OPTION													0
#define DHCP_OPTIONS_SUBNET_MASK												1
#define DHCP_OPTIONS_TIME_OFFSET												2
#define DHCP_OPTIONS_ROUTER_OPTION												3
#define DHCP_OPTIONS_TIME_SERVER_OPTION											4
#define DHCP_OPTIONS_NAME_SERVER_OPTION											5
#define DHCP_OPTIONS_DOMAIN_NAME_SERVER_OPTION									6
#define DHCP_OPTIONS_LOG_SERVER_OPTION											7
#define DHCP_OPTIONS_COOKIE_SERVER_OPTION										8
#define DHCP_OPTIONS_LPR_SERVER_OPTION											9
#define DHCP_OPTIONS_IMPRESS_SERVER_OPTION										10
#define DHCP_OPTIONS_RESOURCE_LOCATION_SERVER_OPTION							11
#define DHCP_OPTIONS_HOST_NAME_OPTION											12
#define DHCP_OPTIONS_BOOT_FILE_SIZE_OPTION										13
#define DHCP_OPTIONS_MERIT_DUMP_FILE											14
#define DHCP_OPTIONS_DOMAIN_NAME												15
#define DHCP_OPTIONS_SWAP_SERVER												16
#define DHCP_OPTIONS_ROOT_PATH													17
#define DHCP_OPTIONS_EXTENSIONS_PATH											18
#define DHCP_OPTIONS_END_OPTION													255

/* IP Layer Parameters per Host */
#define DHCP_OPTIONS_IP_FORWARDING_ENABLE_DISABLE_OPTION						19
#define DHCP_OPTIONS_NON_LOCAL_SOURCE_ROUTING_ENABLE_DISABLE_OPTION				20
#define DHCP_OPTIONS_POLICY_FILTER_OPTION										21
#define DHCP_OPTIONS_MAXIMUM_DATAGRAM_REASSEMBLY_SIZE							22
#define DHCP_OPTIONS_DEFAULT_IP_TIME_TO_LIVE									23
#define DHCP_OPTIONS_PATH_MTU_AGING_TIMEOUT_OPTION								24
#define DHCP_OPTIONS_PATH_MTU_PLATEAU_TABLE_OPTION								25

/* IP Layer Parameters per Interface */
#define DHCP_OPTIONS_INTERFACE_MTU_OPTION										26
#define DHCP_OPTIONS_ALL_SUBNETS_ARE_LOCAL_OPTION								27
#define DHCP_OPTIONS_BROADCAST_ADDRESS_OPTION									28
#define DHCP_OPTIONS_PERFORM_MASK_DISCOVERY_OPTION								29
#define DHCP_OPTIONS_MASK_SUPPLIER_OPTION										30
#define DHCP_OPTIONS_PERFORM_ROUTER_DISCOVERY_OPTION							31
#define DHCP_OPTIONS_ROUTER_SOLICITATION_ADDRESS_OPTION							32
#define DHCP_OPTIONS_STATIC_ROUTE_OPTION										33

/* Link Layer Parameters per Interface */
#define DHCP_OPTIONS_TRAILER_ENCAPSULATION_OPTION								34
#define DHCP_OPTIONS_ARP_CACHE_TIMEOUT_OPTION									35
#define DHCP_OPTIONS_ETHERNET_ENCAPSULATION_OPTION								36

/* TCP Parameters */
#define DHCP_OPTIONS_TCP_DEFAULT_TTL_OPTION										37	
#define DHCP_OPTIONS_TCP_KEEPALIVE_INTERVAL_OPTION								38
#define DHCP_OPTIONS_TCP_KEEPALIVE_GARBAGE_OPTION								39

/* Application and Service Parameters */
#define	DHCP_OPTIONS_NETWORK_INFORMATION_SERVICE_DOMAIN_OPTION					40
#define DHCP_OPTIONS_NETWORK_INFORMATION_SERVERS_OPTION							41
#define DHCP_OPTIONS_NETWORK_TIME_PROTOCOL_SERVERS_OPTION						42
#define DHCP_OPTIONS_VENDOR_SPECIFIC_INFORMATION								43
#define DHCP_OPTIONS_NETBIOS_OVER_TCPIP_NAME_SERVER_OPTION						44
#define DHCP_OPTIONS_NETBIOS_OVER_TCPIP_DATAGRAM_DISTRIBUTION_SERVER_OPTION		45
#define DHCP_OPTIONS_NETBIOS_OVER_TCPIP_NODE_TYPE_OPTION						46
#define DHCP_OPTIONS_NETBIOS_OVER_TCPIP_SCOPE_OPTION							47
#define DHCP_OPTIONS_X_WINDOW_SYSTEM_FONT_SERVER_OPTION							48
#define DHCP_OPTIONS_X_WINDOW_SYSTEM_DISPLAY_MANAGER_OPTION						49
#define DHCP_OPTIONS_NETWORK_INFORMATION_SERVICE_DOMAIN_OPTION					64
#define DHCP_OPTIONS_NETWORK_INFORMATION_SERVICE_SERVERS_OPTION					65
#define DHCP_OPTIONS_MOBILE_IP_HOME_AGENT_OPTION								68
#define	DHCP_OPTIONS_SMTP_SERVER_OPTION											69
#define DHCP_OPTIONS_POP3_SERVER_OPTION											70
#define DHCP_OPTIONS_NNTP_SERVER_OPTION											71
#define DHCP_OPTIONS_WWW_SERVER_OPTION											72
#define DHCP_OPTIONS_DEFAULT_FINGER_SERVER_OPTION								73
#define	DHCP_OPTIONS_IRC_SERVER_OPTION											74
#define DHCP_OPTIONS_STREETTALK_SERVER_OPTION									75
#define DHCP_OPTIONS_STREETTALK_DIRECTORY_ASSISTENCE_SERVER_OPTION				76	

/* DHCP Extensions */
#define DHCP_OPTIONS_REQUESTED_IP_ADDRESS										50
#define DHCP_OPTIONS_IP_ADDRESS_LEASE_TIME										51
#define DHCP_OPTIONS_OPTION_OVERLOAD											52
#define DHCP_OPTIONS_TFTP_SERVER_NAME											66
#define DHCP_OPTIONS_BOOTFILE_NAME												67
#define DHCP_OPTIONS_DHCP_MESSAGE_TYPE											53
#define DHCP_OPTIONS_SERVER_IDENTIFIER											54
#define DHCP_OPTIONS_PARAMETER_ERQUEST_LIST										55
#define DHCP_OPTIONS_MESSAGE													56
#define DHCP_OPTIONS_MAXIMUM_DHCP_MESSAGE_SIZE									57
#define DHCP_OPTIONS_RENEWAL_T1_TIME_VALUE										58
#define DHCP_OPTIONS_REBINDING_T2_TIME_VALUE									59
#define DHCP_OPTIONS_VENDOR_CLASS_IDENTIFIER									60
#define DHCP_OPTIONS_CLIENT_IDENTIFIER											61


typedef struct MAC_Address
{
	u_char		bytes[6];

}MAC_Address;

typedef struct IP_Address
{
	u_char		bytes[4];

}IP_Address;

typedef struct Ethernet_Header
{
	MAC_Address	dstaddr;				// Destination MAC address
	MAC_Address	srcaddr;				// Source MAC address
	u_short		eth_type;				// type

}Ethernet_Header;

typedef struct IP_Header
{
	u_char		ver_headerlen ;			// Version number (4 bits) + header length (4 bits)
	u_char		tos;					// Service type
	u_short		totallen;				// total length
	u_short		identifier;				// logo
	u_short		flags_offset;			// Flag (3 bits) + chip offset (13 bits)
	u_char		ttl;					// survival time
	u_char		protocol;				// Upper layer protocol
	u_short		checksum;				// header checksum
	IP_Address	srcaddr;				// source address
	IP_Address	dstaddr;				// Destination address
	u_int		option_padding;			// Options and padding

}IP_Header;


typedef struct ARP_Header
{
	u_short		hwtype;					// Hardware type
	u_short		ptype;					// agreement type
	u_char		hwlen;					// Hardware length
	u_char		plen;					// protocol length
	u_short		opcode;					// opcode
	MAC_Address	srcmac;					// Source MAC address
	IP_Address	srcip;					// Source IP address
	MAC_Address	dstmac;					// Destination MAC address
	IP_Address	dstip;					// Destination IP address

}ARP_Header;

typedef struct ICMP_Header
{
	u_char		type;					// type
	u_char		code;					// code
	u_short		chksum;					// Checksum
	u_int		others;					// Other parts of the header (the corresponding content is determined by the message type)

}ICMP_Header;

typedef struct TCP_Header
{
	u_short		srcport;				// source port
	u_short		dstport;				// destination port
	u_int		seq;					// serial number
	u_int		ack;					// Confirmation Number
	u_short		headerlen_rsv_flags;	// Header length (4 bits) + reserved (6 bits) +
										// URG(1 bit) + ACK(1 bit) + PSH(1 bit) + RST(1 bit) + SYN(1 bit) + FIN(1 bit)
	u_short		win_size;				// window size
	u_short		chksum;					// Checksum
	u_short		urg_ptr;				// emergency pointer
	u_int		option;					// Options

}TCP_Header;

typedef struct UDP_Header
{
	u_short		srcport;				// source port
	u_short		dstport;				// destination port
	u_short		len;					// length
	u_short		checksum;				// Checksum

}UDP_Header;

typedef struct DNS_Header
{
	u_short		identifier;				// logo
	u_short		flags;					// logo
	u_short		questions;				// Query the number of records
	u_short		answer_RRs;				// Number of answer records
	u_short		authority_RRs;			// Number of authorized answer records
	u_short		additional_RRs;			// Number of additional information records

}DNS_Header;

typedef struct DNS_Query
{
//	char*		name					// Domain name (variable length)
	u_short		type;					// Query type
	u_short		classes;				// Query class

}DNS_Query;

typedef struct DNS_ResourceRecord
{
//	char*		name					// Domain name (variable length)
	u_short		type;					// type
	u_short		classes;				// kind
	u_int		ttl;					// survival time
//	u_short		data_length				// Resource data length
//	char*		data					// Resource data (variable length)

}DNS_ResourceRecord;

typedef struct DHCP_Header
{
	u_char		op;						// Message type
	u_char		htype;					// Hardware type
	u_char		hlen;					// Hardware address length
	u_char		hops;					// Hop count
	u_int		xid;					// Transaction ID
	u_short		secs;					// Client start time
	u_short		flags;					// logo
	IP_Address	ciaddr;					// Client IP address
	IP_Address	yiaddr;					// your IP address
	IP_Address	siaddr;					// Server IP address
	IP_Address	giaddr;					// Gateway IP address
	u_char		chaddr[16];				// Customer hardware address
	u_char		snamer[64];				// Server hostname
	u_char		file[128];				// startup file name
//  char*		options					// Options (variable length)
	
}DHCP_Header;



