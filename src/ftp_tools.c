# include <ft_arp.h>

int is_ftp_command(const unsigned char *payload, int length) {
    if (length < 4) return 0;
    

    if (strncmp((char *)payload, "STOR", 4) == 0 || strncmp((char *)payload, "RETR", 4) == 0) {
        return 1;
    }
    
    return 0;
}

void extract_filename(const unsigned char *payload, int length) {
    char *filename = (char *)payload + 5;
    
    printf("Filename: ");
    for (int i = 0; i < length - 5; i++) {
        if (isprint(filename[i])) {
            printf("%c", filename[i]);
        } else {
            break;
        }
    }
    printf("\n");
}