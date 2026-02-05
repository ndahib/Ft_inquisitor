#include <42_arp.h>

int	is_valid_ip_addr(const char *ipV4)
{
	// size_t len;
	int octet;
	char *tokens;
	int octet_count;
	char *dup_ipV4;
	octet_count = 0;
	
	printf("Checking ip address: %s\n", ipV4);
	// len = strlen(ipV4);
	// if (len > 15 || len < 7)
	// 	print_error("%s is Invalid: the ipV4 format should be alwyas x.x.x.x");
	dup_ipV4 = strdup(ipV4);
	tokens = strtok(dup_ipV4, (const char *)("."));
	while (tokens != NULL){
		if (octet_count >= 4)
		{
			free(dup_ipV4);
			while(1);
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