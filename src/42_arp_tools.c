#include <42_arp.h>

void	is_valid_ip_addr(char *ipV4)
{
	size_t len;
	size_t i;
	int octet;
	char *tokens;
	int octet_count;
	const char del[2] = ".";
	
	octet_count = 0;
	
	print_operation(strcat("Cheking ip address: ", ipV4), 0);
	
	len = strlen(ipV4);
	if (len > 15 || len < 7)
	{
		print_error("%s is Invalid: the ipV4 format should be alwyas x.x.x.x");
	}
	tokens = strtok(ipV4, del);

	while (tokens != NULL){
		i = 0;
		if (octet_count >= 4)
		{
			print_error("Invalid IP: there should be exactly 4 octets");
		}
		octet = atoi(tokens);
		if (octet < 0 && octet > 255){
			print_error("%s is Invalid: each octet in ipV4 address should be ranged from 0 to 255");
		};
		while (i < strlen(tokens))
		{
			if (tokens[i] < '0' || tokens[i] > '9') {
				free(tokens);
				print_error("Invalid IP: octet contains non-numeric characters");
			}
			i++;
		}
		octet_count++;
		tokens = strtok(NULL, del);

	}
	if (octet_count != 4)
	{
		free(tokens);
		print_error("Invalid IP: there should be exactly 4 octets");
	}
	free(tokens);

}

void	is_valid_mac_addr(char *mac)
{
	size_t len;
	// size_t i;
	// int digits;
	char *tokens;
	int	digit_count;
	const char del[2] = ".";
	

	digit_count = 0;
	
	print_operation(strcat("Cheking ip address: ", mac), 0);
	
	len = strlen(mac);
	if (len > 17)
	{
		print_error("%s is Invalid: the mac format should be alwyas xx.xx.xx.xx.xx.xx");
		print_error("Example: 00:1b:63:84:45:e6");
	}
	tokens = strtok(mac, del);

	while (tokens != NULL){
		// i = 0;
		if (digit_count >= 6)
		{
			print_error("Invalid MAC: there should be exactly 6 six pairs of two-digit");
		}
		// digits = atoi(tokens);
		// if (digits < 0 && digits > 255){
		// 	print_error("%s is Invalid: each octet in ipV4 address should be ranged from 0 to 255");
		// };
		// while (i < strlen(tokens))
		// {
		// 	if (tokens[i] < '0' || tokens[i] > '9') {
		// 		free(tokens);
		// 		print_error("Invalid IP: octet contains non-numeric characters");
		// 	}
		// 	i++;
		// }
		digit_count++;
		tokens = strtok(NULL, del);

	}
	if (digit_count != 6)
	{
		free(tokens);
		print_error("Invalid Mac: there should be exactly 6 six pairs of two-digit");
	}
	free(tokens);

}