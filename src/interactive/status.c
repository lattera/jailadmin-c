#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "jailadmin.h"

void print_status(JAILADMIN *, jabool);

int num_args(char **cmd)
{
    int numargs;
    for (numargs=0; cmd[numargs] != NULL; numargs++)
        ;
    return numargs;
}

CALLBACK_RETURN status_callback(JAILADMIN *admin, char **cmd)
{
    jabool infinite=jafalse;
    int ch, numargs;

    if (strcasecmp(cmd[0], "status"))
        return CONT_PROC;

    numargs = num_args(cmd);
    optreset = 1;
    optind = 1;
    while ((ch = getopt(numargs, cmd, "i")) != -1) {
        switch ((char)ch) {
            case 'i':
                infinite=jatrue;
                break;
        }
    }

    print_status(admin, infinite);

    return TERM_PROC;
}

void print_status(JAILADMIN *admin, jabool infinite)
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
        wclear(admin->windows[0]);
        wmove(admin->windows[0], 0, 0);
        for (i=0; jails[i] != NULL; i++) {
            if (has_colors()) {
                wprintw(admin->windows[0], "[");
                wattron(admin->windows[0], is_jail_fully_online(jails[i]) ? COLOR_PAIR(2) : COLOR_PAIR(1));
                wprintw(admin->windows[0], "%s", jails[i]->name);
                wattroff(admin->windows[0], is_jail_fully_online(jails[i]) ? COLOR_PAIR(2) : COLOR_PAIR(1));
                wprintw(admin->windows[0], "]\n");
            } else {
                wprintw(admin->windows[0], "[%s] => %s\n", jails[i]->name, (is_jail_fully_online(jails[i])) ? "online" : "offline");
            }

            wrefresh(admin->windows[0]);
        }

        if (infinite == jatrue)
            sleep(10);
    } while (infinite == jatrue);
}
