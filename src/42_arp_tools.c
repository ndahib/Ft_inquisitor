#include <42_arp.h>

int	is_valid_ip_addr(const char *ipV4)
{
	size_t len;
	int octet;
	char *tokens;
	int octet_count;
	char *dup_ipV4;
	octet_count = 0;
	
	printf("Checking ip address: %s\n", ipV4);
	len = strlen(ipV4);
	if (len > 15 || len < 7)
		print_error("Invalid: the ipV4 format should be alwyas x.x.x.x");
	dup_ipV4 = strdup(ipV4);
	if (!dup_ipV4) {
    print_error("Memory allocation failed");
}
	tokens = strtok(dup_ipV4, (const char *)("."));
	while (tokens != NULL){
		if (octet_count >= 4)
		{
			free(dup_ipV4);
			print_error("Invalid IP: there should be exactly 4 octets");
		}
		for (size_t i = 0; i < strlen(tokens); i++)
		{	
			if (!isdigit(tokens[i]))
			{
				free(dup_ipV4);
				print_error("Invalid IP: octet contains non-numeric characters");
			}
		}
		octet = atoi(tokens);
		if (octet < 0 || octet > 255){
			free(dup_ipV4);
			print_error("Invalid IP: each octet in ipV4 address should be ranged from 0 to 255");
		};
		octet_count++;
		tokens = strtok(NULL, (const char *)("."));

	}
	if (octet_count != 4)
	{
		free(dup_ipV4);
		print_error("Invalid IP: there should be exactly 4 octets");
	}
	free(dup_ipV4);
	return (1);
}

int is_valid_mac_addr(const char *mac)
{
    size_t len;
    int octet;
    char *tokens;
    int octet_count;
    char *dup_mac;
    
    octet_count = 0;
    printf("Checking MAC address: %s\n", mac);


    len = strlen(mac);
    if (len != 17)
        print_error("%s is Invalid: a MAC address should be exactly 17 characters long");


    dup_mac = strdup(mac);
    tokens = strtok(dup_mac, ":");
    while (tokens != NULL)
    {
        if (octet_count >= 6)
        {
            free(dup_mac);
            print_error("Invalid MAC address: there should be exactly 6 octets");
        }
        if (strlen(tokens) != 2)
        {
            free(dup_mac);
            print_error("Invalid MAC address: each octet should be exactly 2 characters long");
        }

        for (size_t i = 0; i < strlen(tokens); i++)
        {
            if (!isxdigit(tokens[i]))
            {
                free(dup_mac);
                print_error("Invalid MAC address: octet contains non-hexadecimal characters");
            }
        }

        octet = (int)strtol(tokens, NULL, 16);
        if (octet < 0 || octet > 255)
        {
            free(dup_mac);
            print_error("Invalid MAC address: each octet in a MAC address should be in the range 00 to FF");
        }

        octet_count++;
        tokens = strtok(NULL, ":");
    }

    if (octet_count != 6)
    {
        free(dup_mac);
        print_error("Invalid MAC address: there should be exactly 6 octets");
    }

    free(dup_mac);
    return 1;
}

void    fill_packet(t_arp_packet *packet, char **av)
{
    uint8_t				source_ip[4];
    uint8_t				target_ip[4];
	struct ether_addr	*source_eth_addr;
	struct ether_addr	*target_eth_addr;

	source_eth_addr = NULL;
	target_eth_addr = NULL;
    if (inet_pton(AF_INET, av[1], source_ip) != 1){
		fprintf(stderr, "Invalid IPv4 format %s: \n", av[1]);
		exit(EXIT_FAILURE);
	}
	if (inet_pton(AF_INET, av[3], target_ip) != 1)
	{
		fprintf(stderr, "Invalid IPv4 format %s: \n", av[3]);
		exit(EXIT_FAILURE);		
	}
    source_eth_addr = ether_aton(av[2]);
	if (source_eth_addr == NULL)
	{
		fprintf(stderr, "Invalid Mac format %s: \n", av[2]);
		exit(EXIT_FAILURE);	
	}
	memcpy(packet->eth_header.ethernet_source_addr, 
		   source_eth_addr->ether_addr_octet, ETH_ALEN);
	packet->eth_header.ethernet_frame_type = htons(ETHERTYPE_ARP);
	memcpy(packet->sender_eth_addr, source_eth_addr->ether_addr_octet, ETH_ALEN);

	target_eth_addr = ether_aton(av[4]);
	if (target_eth_addr == NULL)
	{
		fprintf(stderr, "Invalid Mac format %s: \n", av[4]);
		exit(EXIT_FAILURE);	
	}
	memcpy(packet->eth_header.ethernet_destination_addr, 
		target_eth_addr->ether_addr_octet, ETH_ALEN);
	memcpy(packet->target_eth_addr, target_eth_addr->ether_addr_octet, ETH_ALEN);
	

    packet->hard_type = htons(ARPHRD_ETHER);
    packet->hard_size = 6;
    packet->prot_type = htons(ETHERTYPE_IP);
    packet->prot_size = 4;
    packet->operation = htons(REPLY);

    memcpy(packet->sender_ip_addr, source_ip, 4);
    memcpy(packet->target_ip_addr, target_ip, 4);
}

int    create_socket(const char *if_name)
{
    int sock_fd;
    struct ifreq if_idx;
    
    sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(sock_fd < 0)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, if_name, IFNAMSIZ - 1);
    if (ioctl(sock_fd, SIOCGIFINDEX, &if_idx) < 0)
    {
        perror("SIOCGIFINDEX");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_idx.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ARP);
    
    if (bind(sock_fd, (struct sockaddr*)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Socket created and bound to interface %s (index: %d)\n", 
           if_name, if_idx.ifr_ifindex);
    
    return (sock_fd);
}
void send_packet(t_arp_packet *packet, int sock_fd)
{
	const char			*if_name;
    ssize_t				sent_bytes;
    struct sockaddr_ll	sock_address;

    if_name = "eth0";
    memset(&sock_address, 0, sizeof(struct sockaddr_ll));

    sock_address.sll_family = PF_PACKET;
    sock_address.sll_protocol = htons(ETH_P_ARP);
    sock_address.sll_ifindex = if_nametoindex(if_name);
	memcpy(sock_address.sll_addr, packet->eth_header.ethernet_destination_addr, ETH_ALEN);

    if (sock_address.sll_ifindex == 0)
    {
        perror("Interface eth0 not found");
        close(sock_fd);
        return;
    }

    sent_bytes = sendto(sock_fd, packet, sizeof(*packet),
                        0,
                        (struct sockaddr *)&sock_address,
                        sizeof(sock_address));
    if (sent_bytes == -1)
    {
        perror("sendto");
        return;
    }
	printf("Sent ARP packet (%ld bytes) on interface %s\n", sent_bytes, if_name);
    printf("  Destination MAC: ");
    print_mac(packet->eth_header.ethernet_destination_addr);
    printf("\n");
}