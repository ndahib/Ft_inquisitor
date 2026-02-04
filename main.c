#include <stdio.h>
#include <42_arp.h>

int main(int ac, char **av)
{
    
    if (ac != 5){
        perror("Please enter the Ip source, the Mac source, the target Ip source, The Mac source \n");
        perror("Examples :192.168.10.45, aa.bb.cc.dd.ee.ff, 192.168.10.47, a0.b0.c0.d0.e0.f0\n");
    }
    
    is_valid_ip(av[1]);
    is_valid_ip(av[3]);
    is_valid_mac(av[2]);
    is_valid_mac(av[4]);

    
    return (1);
}