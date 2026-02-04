
void	is_valid_ip(char *ipV4)
{
	int len;
	int i;
	int octet;
	char **tokens;
	int octet_count;
	
	i = 0;
	octet_count = 0;
	
	print_operation("Cheking ip address: %s", ipV4);
	
	len = ft_strlen(ipV4);
	if (len > 15 || len < 7)
	{
		print_error("%s is Invalid: the ipV4 format should be alwyas x.x.x.x");
	}
	tokens = ft_split(ipV4, '.');

	while (*tokens != NULL){
		i = 0;
		if (octet_count >= 4)
		{
			print_error("Invalid IP: there should be exactly 4 octets");
		}
		octet = ft_atoi(*tokens);
		if (octet < 0 && octet > 255){
			print_error("%s is Invalid: each octet in ipV4 address should be ranged from 0 to 255");
		};
		while (i < ft_strlen(*tokens))
		{
			if (token[i] < '0' || token[i] > '9') {
				free(tokens); // to correct later
				print_error("Invalid IP: octet contains non-numeric characters");
			}
			i++;
		}
		*tokens++;
		octet_count++;

	}
	if (octet_count != 4)
	{
		free(tokens);
		print_error("Invalid IP: there should be exactly 4 octets");
	}
	free(tokens);

}

void	is_valid_mac()
{
	int len;
	int i;
	int octet;
	char **tokens;
	int octet_count;
	
	i = 0;
	octet_count = 0;
	
	print_operation("Cheking ip address: %s", ipV4);
	
	len = ft_strlen(ipV4);
	if (len > 15 || len < 7)
	{
		print_error("%s is Invalid: the ipV4 format should be alwyas x.x.x.x");
	}
	tokens = ft_split(ipV4, '.');

	while (*tokens != NULL){
		i = 0;
		if (octet_count >= 4)
		{
			print_error("Invalid IP: there should be exactly 4 octets");
		}
		octet = ft_atoi(*tokens);
		if (octet < 0 && octet > 255){
			print_error("%s is Invalid: each octet in ipV4 address should be ranged from 0 to 255");
		};
		while (i < ft_strlen(*tokens))
		{
			if (token[i] < '0' || token[i] > '9') {
				free(tokens); // to correct later
				print_error("Invalid IP: octet contains non-numeric characters");
			}
			i++;
		}
		*tokens++;
		octet_count++;

	}
	if (octet_count != 4)
	{
		free(tokens);
		print_error("Invalid IP: there should be exactly 4 octets");
	}
	free(tokens);

}