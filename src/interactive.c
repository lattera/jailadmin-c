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

    jails = get_jails(admin);
    while (1) {
        move(0,0);
        for (i=0; jails[i] != NULL; i++) {
            printw("[%s] %s\n", jails[i]->name, (is_jail_fully_online(jails[i])) ? "true" : "false");
            refresh();
        }

        sleep(10);
    }

    endwin();
}
