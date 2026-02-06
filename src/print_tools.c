#include <42_arp.h>
bool g_silence_mode = false;

void	print_error(char *msg)
{
  printf("\033[1;31m[ERROR]: %s\033[0m\n", msg);
  exit(EXIT_FAILURE);
}

void	print_operation(char *msg, int flag)
{
  if (flag)
    return;
  if (g_silence_mode)
  {
	  printf(msg);
  }
}

void    print_mac(u_int8_t *mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void    print_ip(u_int8_t *ip) {
    printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

void print_packet_info(t_arp_packet *packet) {
    printf("\n=== ARP Packet Information ===\n");
    printf("Ethernet Destination: ");
    print_mac(packet->eth_header.ethernet_destination_addr);
    printf("\nEthernet Source: ");
    print_mac(packet->eth_header.ethernet_source_addr);
    printf("\nFrame Type: 0x%04x\n", ntohs(packet->eth_header.ethernet_frame_type));
    
    printf("\nHardware Type: 0x%04x\n", ntohs(packet->hard_type));
    printf("Protocol Type: 0x%04x\n", ntohs(packet->prot_type));
    printf("Operation: %s (0x%04x)\n", 
            "REPLY",
           ntohs(packet->operation));
    
    printf("Sender MAC: ");
    print_mac(packet->sender_eth_addr);
    printf("\nSender IP: ");
    print_ip(packet->sender_ip_addr);
    
    printf("\nTarget MAC: ");
    print_mac(packet->target_eth_addr);
    printf("\nTarget IP: ");
    print_ip(packet->target_ip_addr);
    printf("\n=============================\n");
}