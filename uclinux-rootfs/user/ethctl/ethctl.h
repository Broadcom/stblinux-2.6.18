/*
 */

#ifndef _ETHCTL_H
#define _ETHCTL_H

#include <linux/if.h>

/* media type command argument option */
#define MEDIA_TYPE_AUTO         0
#define MEDIA_TYPE_100M_FD      1
#define MEDIA_TYPE_100M_HD      2
#define MEDIA_TYPE_10M_FD       3
#define MEDIA_TYPE_10M_HD       4

typedef struct command cmd_t;
typedef int (cmd_func_t)(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv);

struct command
{
    int         nargs;
    const char  *name;
    cmd_func_t  *func;
    const char  *help;
};

cmd_t *command_lookup(const char *cmd);
void command_help(const cmd_t *);
void command_helpall(void);

#endif
