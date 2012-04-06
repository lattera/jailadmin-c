#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "jailadmin.h"

void print_status(JAILADMIN *, void *, jabool);

int num_args(char **cmd)
{
    int numargs;
    for (numargs=0; cmd[numargs] != NULL; numargs++)
        ;
    return numargs+1;
}

CALLBACK_RETURN status_callback(JAILADMIN *admin, char **cmd, void *scr)
{
    jabool infinite=jafalse;
    int ch, numargs;

    if (strcasecmp(cmd[0], "status"))
        return CONT_PROC;

    numargs = num_args(cmd);
    while ((ch = getopt(numargs, cmd, "i")) != -1) {
        wprintw(scr, "getopt returned: %c\n", (char)ch);
        switch ((char)ch) {
            case 'i':
                infinite=jatrue;
                break;
        }
    }

    print_status(admin, scr, infinite);

    return TERM_PROC;
}

void print_status(JAILADMIN *admin, void *scr, jabool infinite)
{
    JAIL **jails;
    unsigned long i;

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
    }

    jails = get_jails(admin);
    do {
//        wclear(scr);
//        wmove(scr, 0, 0);
        for (i=0; jails[i] != NULL; i++) {
            if (has_colors()) {
                wprintw(scr, "[");
                wattron(scr, is_jail_fully_online(jails[i]) ? COLOR_PAIR(2) : COLOR_PAIR(1));
                wprintw(scr, "%s", jails[i]->name);
                wattroff(scr, is_jail_fully_online(jails[i]) ? COLOR_PAIR(2) : COLOR_PAIR(1));
                wprintw(scr, "]\n");
            } else {
                wprintw(scr, "[%s] => %s\n", jails[i]->name, (is_jail_fully_online(jails[i])) ? "online" : "offline");
            }
        }

        if (infinite == jatrue) {
            wprintw(scr, "Refreshing in 10 seconds\n");
            sleep(10);
        }

        refresh();

    } while (infinite == jatrue);
}
