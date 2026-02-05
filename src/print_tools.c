#include <42_arp.h>
bool g_silence_mode = false;

void	print_error(char *msg)
{
  printf("\033[1;31m[ERROR]: %s\033[0m\n", msg);
  exit(EXIT_FAILURE);
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