/*
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "ethctl.h"

static void help()
{
    fprintf(stderr, "Usage: ethctl <interface> <command> [arguments...]\n\n");
    fprintf(stderr, "commands:\n");
    command_helpall();
}

/* brcm begin */
#ifdef BUILD_STATIC
int ethctl_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
/* brcm end */
{
    cmd_t *cmd;
    struct ifreq ifr;
    int skfd;
    int rc;

    if (argc < 3) {
        help();
        return -1;
    }

    cmd = command_lookup(argv[2]);
    if (cmd == NULL) {
        fprintf(stderr, "invalid command [%s]\n", argv[2]);
        help();
        return -1;
    }
    
    if (argc < cmd->nargs + 3) {
        fprintf(stderr, "incorrect number of arguments for command\n");
        command_help(cmd);
        return -1;
    }

    if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        fprintf(stderr, "socket open error\n");
        return -1;
    }

    if (strncmp(argv[1], "eth", strlen("eth")) == 0) {
        strcpy(ifr.ifr_name, argv[1]);
        if ( ioctl(skfd, SIOCGIFINDEX, &ifr) < 0 ) {
            fprintf(stderr, "invalid interface name %s\n", argv[1]);
            command_help(cmd);
            close(skfd);
            return -1;
        }
    } else {
        fprintf(stderr, "invalid interface name %sx\n", argv[1]);
        command_help(cmd);
        close(skfd);
        return -1;
    }

    rc = cmd->func(skfd, &ifr, cmd, argv);

    close(skfd);

    return rc;
}
