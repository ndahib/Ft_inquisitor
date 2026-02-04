#include <42_arp.h>

void	print_error(char *msg)
{
	perror(msg);
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