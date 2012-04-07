#include "jailadmin.h"

CALLBACK_RETURN jail_cmd(JAILADMIN *admin, char **cmd)
{
    int numargs;
    int ch;
    char *jailname;
    JAIL *jail;

    if (strcasecmp(cmd[0], "jail"))
        return CONT_PROC;

    optreset = optind = 1;
    numargs = num_args(cmd);
    while ((ch = getopt(numargs, cmd, "s:S:c:")) != -1) {
        switch ((char)ch) {
            case 's':
                jail = get_jail(admin, optarg);
                if ((jail)) {
                    if (start_jail(admin, jail) == jatrue)
                        wprintw(admin->windows[MAINWIN], "Jail %s started successfully\n", optarg);
                    else
                        wprintw(admin->windows[MAINWIN], "Jail %s failed to start\n", optarg);
                }
                else
                    wprintw(admin->windows[MAINWIN], "No such jail: %s\n", optarg);
                break;
            case 'S':
                jail = get_jail(admin, optarg);
                if ((jail)) {
                    if (stop_jail(admin, jail) == jatrue)
                        wprintw(admin->windows[MAINWIN], "Jail %s stopped successfully\n", optarg);
                    else
                        wprintw(admin->windows[MAINWIN], "Jail %s failed to stop\n", optarg);
                }
                else
                    wprintw(admin->windows[MAINWIN], "No such jail: %s\n", optarg);
                break;
            case 'c':
                /* config jail */
                break;
        }
    }

    wrefresh(admin->windows[MAINWIN]);

    return CONT_PROC;
}
