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
#include <pcap/pcap.h>

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

void	print_error(char *msg);
void	print_operation(char *msg, int flag);
int		is_valid_ip_addr(const char *ipV4);
int		is_valid_mac_addr(const char *mac);
void    fill_packet(t_arp_packet *packet, char **av);
void	print_packet_info(t_arp_packet *packet);
void	send_packet(t_arp_packet *packet, int sock_fd);
int		create_socket(const char *if_name);
void    print_mac(u_int8_t *mac);
void	pcap_send_packet(t_arp_packet *packet);