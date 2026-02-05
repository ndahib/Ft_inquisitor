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
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>


#include <pcap.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#define	REQUEST = 0x001;
#define	REPLY = 0x002;
#define	RARP_REQUEST = 0x003;
#define	RARP_REPLY = 0x004;

extern bool g_silence_mode;

typedef struct s_eth_header
{
   u_int8_t		ethernet_destination_addr[ETH_ALEN]; 	//mac
   u_int8_t		ethernet_source_addr[ETH_ALEN];      	//mac
   u_int16_t	ethernet_frame_type; 		            // ETH_ARP   
}	t_eth_header;

typedef struct s_arp_packet
{
	u_int16_t	hard_type;
	u_int16_t	prot_type;
	u_int8_t	hard_size;
	u_int8_t	prot_size;
	u_int16_t	operation;
	u_int8_t	sender_eth_addr[ETH_ALEN]; 	//mac
	u_int8_t	sender_ip_addr[4]; 		    //ip
	u_int8_t	target_eth_addr[6]; 	    //mac
	u_int8_t	target_ip_addr[4]; 		    //ip
	t_eth_header	*eth_header;
}	t_arp_packet;

void	print_error(char *msg);
void	print_operation(char *msg, int flag);
int		is_valid_ip_addr(const char *ipV4);
int		is_valid_mac_addr(const char *mac);