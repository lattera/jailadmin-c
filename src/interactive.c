#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include <sys/types.h>
#include <signal.h>

#include "jailadmin.h"

void sighandler(int signo)
{
    switch (signo) {
        case SIGINT:
            endwin();
            exit(0);
    }
}

void interactive(JAILADMIN *admin)
{
    JAIL **jails;
    unsigned long i;

    signal(SIGINT, sighandler);

    initscr();

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
    }

    jails = get_jails(admin);
    while (1) {
        move(0,0);
        for (i=0; jails[i] != NULL; i++) {
            if (has_colors()) {
                printw("[");
                if (is_jail_fully_online(jails[i]))
                    attron(COLOR_PAIR(2));
                else
                    attron(COLOR_PAIR(1));

                printw("%s", jails[i]->name);

                if (is_jail_fully_online(jails[i]))
                    attroff(COLOR_PAIR(2));
                else
                    attroff(COLOR_PAIR(1));
                printw("]\n");
            } else {
                printw("[%s] %s\n", jails[i]->name, (is_jail_fully_online(jails[i])) ? "online" : "offline");
            }

            refresh();
        }

        sleep(10);
    }

    endwin();
}
