#include <stdio.h>
#include <42_arp.h>

int main(int ac, char **av)
{
	int sock_fd;
    t_arp_packet		arp_packet;
	const char			*interface;

	interface = "eth0";
	if (ac != 5){
		print_error("Please enter the Ip source, the Mac source, the target Ip source, The Mac source.\n -> Examples :192.168.10.45, aa.bb.cc.dd.ee.ff, 192.168.10.47, a0.b0.c0.d0.e0.f0\n");
	}
    
	is_valid_ip_addr(av[1]);
	is_valid_ip_addr(av[3]);
	is_valid_mac_addr(av[2]);
	is_valid_mac_addr(av[4]);

	memset(&arp_packet, 0, sizeof(t_arp_packet));

	fill_packet(&arp_packet, av);
	print_packet_info(&arp_packet);

	sock_fd = create_socket(interface);
	send_packet(&arp_packet, sock_fd);

	close(sock_fd);
	return (0);
}