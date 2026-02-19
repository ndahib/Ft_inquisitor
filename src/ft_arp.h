/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42_arp.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndahib <ndahib@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:51:36 by ndahib            #+#    #+#             */
/*   Updated: 2026/02/04 12:00:09 by ndahib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <linux/if.h>
#include <pcap.h>
#include <signal.h>
#include <netinet/tcp.h>

#define	REQUEST  0x001
#define	REPLY  0x002
#define	RARP_REQUEST 0x003
#define	RARP_REPLY  0x004
#define FRAME_ARP_REQUEST  0x0800
#define FRAME_ARP_REPLY  0x0806

extern bool g_silence_mode;


typedef struct s_eth_header
{
   u_int8_t		ethernet_destination_addr[ETH_ALEN];	//mac
   u_int8_t		ethernet_source_addr[ETH_ALEN];			//mac
   u_int16_t	ethernet_frame_type;					//ETH_ARP   
}	t_eth_header;

typedef struct s_arp_packet
{
	t_eth_header	eth_header;
	u_int16_t	hard_type;
	u_int16_t	prot_type;
	u_int8_t	hard_size;
	u_int8_t	prot_size;
	u_int16_t	operation;
	u_int8_t	sender_eth_addr[ETH_ALEN];	//mac
	u_int8_t	sender_ip_addr[4];			//ip
	u_int8_t	target_eth_addr[ETH_ALEN];	//mac
	u_int8_t	target_ip_addr[4];			//ip
}	t_arp_packet;

struct ip_header {
    unsigned char ip_hl:4;
    unsigned char ip_v:4;
    unsigned char ip_tos;
    unsigned short ip_len;
    unsigned short ip_id;
    unsigned short ip_off;
    unsigned char ip_ttl;
    unsigned char ip_protocol;
    unsigned short ip_sum;
    unsigned int ip_src;
    unsigned int ip_dst;
};

struct tcp_header {
    unsigned short th_sport;      // source port
    unsigned short th_dport;      // destination port
    unsigned int th_seq;          // sequence number
    unsigned int th_ack;          // acknowledgement number
    unsigned char th_off:4;       // data offset
    unsigned char th_flags;       // flags
    unsigned short th_win;        // window
    unsigned short th_sum;        // checksum
    unsigned short th_urp;        // urgent pointer
};

// == PRINT FUNCTIONS =========================================================
void	print_error(char *msg);
void	print_operation(char *msg, int flag);
void	print_packet_info(t_arp_packet *packet);
void    print_mac(u_int8_t *mac);
void    print_ip(u_int8_t *ip);
void	dump_packet_hex(t_arp_packet *packet);

// == ARP FUNCTION ============================================================
void	pcap_send_packet(t_arp_packet *packet, pcap_t *handle);
void	fill_packet(t_arp_packet *packet,const char* src_ip, 
        const char* src_mac, const char* target_ip, const char* target_mac);
void    packet_handler(unsigned char *user_data, 
        const struct pcap_pkthdr *pkthdr, const unsigned char *packet);

// == HELPER FUNCTION =========================================================
int		is_valid_ip_addr(const char *ipV4);
int		is_valid_mac_addr(const char *mac);
char	*get_default_gateway_ip(const char	*interface);
char	*get_default_gateway_mac(const char	*gateway_ip);

// == FTP FUNCTION ============================================================
void    extract_filename(const unsigned char *payload, int length);
int     is_ftp_command(const unsigned char *payload, int length);