#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>

#include <sys/types.h>
#include <signal.h>

#include "jailadmin.h"

#define BUFSZ 512
#define NUMPLUGINS 1

CALLBACK_RETURN status_callback(JAILADMIN *, char **);
CALLBACK_RETURN jail_cmd(JAILADMIN *, char **);

typedef struct _interactive_plugin {
    char *name;
    interactive_callback callback;
} INTERACTIVE_PLUGIN;

INTERACTIVE_PLUGIN plugins[] = {
    {
        "status",
        status_callback
    },
    {
        "jail",
        jail_cmd
    },
    {
        NULL,
        NULL,
    }
};

void sighandler(int signo)
{
    switch (signo) {
        case SIGINT:
            endwin();
            exit(0);
    }
}

int num_args(char **cmd)
{
    unsigned long i;

    for (i=0; cmd[i] != NULL; i++)
        ;

    return i;
}

void create_windows(JAILADMIN *admin)
{
    WINDOW *mainwin;
    WINDOW *inputwin;

    mainwin = newwin(LINES-1, COLS, 0, 0);
    inputwin = newwin(1, COLS, LINES-1, 0);

    admin->windows = xmalloc(NUMWINS * sizeof(WINDOW **));
    admin->windows[MAINWIN] = mainwin;
    admin->windows[INPUTWIN] = inputwin;
}

size_t readcmd(void *scr, char *buf, size_t bufsz) {
    size_t i;
    int ch;

    memset(buf, 0x00, bufsz);
    for (i=0; i<bufsz; i++) {
        switch ((ch = wgetch(scr))) {
            case '\n':
                return i;
            default:
                buf[i] = ch;
                break;
        }
    }

    return i;
}

char **parse_cmd(char *cmd)
{
    char *p1,*p2, **ret=NULL;
    unsigned long i=0;

    p1 = cmd;
    while (*p1) {
        while (isspace(*p1))
            p1++;

        p2 = p1;
        while (!isspace(*p2) && *(p2++))
            ;

        if (*p2 == 0x00)
            break;

        *p2 = 0x00;

        ret = realloc(ret, ++i * sizeof(char **));
        ret[i-1] = p1;

        p1 = p2+1;
    }

    ret = realloc(ret, ++i * sizeof(char **));
    ret[i-1] = p1;

    ret = realloc(ret, ++i * sizeof(char **));
    ret[i-1] = NULL;

    return ret;
}

void interactive(JAILADMIN *admin)
{
    JAIL **jails;
    unsigned long i;
    char cmd[BUFSZ+1];
    char **parsed;

    signal(SIGINT, sighandler);

    initscr();
    create_windows(admin);

    while (1) {
        readcmd(admin->windows[INPUTWIN], cmd, BUFSZ);
        wclear(admin->windows[INPUTWIN]);
        wmove(admin->windows[INPUTWIN], 0, 0);

        if (!strlen(cmd) || cmd[0] == '#')
            continue;

        if (!strcmp(cmd, "exit"))
            break;

        parsed = parse_cmd(cmd);
        for (i=0; plugins[i].name != NULL; i++) {
            if (plugins[i].callback(admin, parsed) == TERM_PROC)
                break;
        }
    }

    endwin();
}
