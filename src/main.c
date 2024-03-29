#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>

#include "jailadmin.h"

void usage(char *name)
{
    fprintf(stderr, "USAGE: %s [-i <ini>] [-I]\n", name);
    fprintf(stderr, "-I: Interactive mode\n\n");
    fprintf(stderr, "Environment variables that can be set:\n");
    fprintf(stderr, "JAILADMIN_CONFIG\t- INI file for config.\n");
    fprintf(stderr, "    If set, do not pass file name as an argument to the program\n");
    exit(1);
}

JAILADMIN *init_jailadmin(int argc, char *argv[])
{
    JAILADMIN *admin;
    char *inifile;
    JAIL *jail;
    int ch;

    admin = xmalloc(sizeof(JAILADMIN));

    inifile = getenv("JAILADMIN_CONFIG");
    while ((ch = getopt(argc, argv, "Ii:")) != -1) {
        switch ((char)ch) {
            case 'i':
                inifile = optarg;
                break;
            case 'I':
                admin->interactive = jatrue;
                break;
            default:
                usage(argv[0]);
        }
    }

    if (!(inifile))
        usage(argv[0]);

    admin->ini = parse_ini(inifile);
    if (!(admin->ini)) {
        fprintf(stderr, "[-] ini is null!\n");
        return 0;
    }

    admin->db = get_section(admin->ini, "db");
    admin->ctx = init_sql(get_section_var(admin->db, "host"),
                       get_section_var(admin->db, "user"),
                       get_section_var(admin->db, "password"),
                       get_section_var(admin->db, "db")
                 );

    if (!(admin->ctx))
        exit(1);

    admin->prefix = get_section_var(admin->db, "prefix");
    if (!(admin->prefix))
        admin->prefix = "";

    install_schema(admin);

    return admin;
}

int main(int argc, char *argv[])
{
    JAILADMIN *admin;
    admin = init_jailadmin(argc, argv);

    if (admin->interactive)
        interactive(admin);

    close_sql(admin->ctx, jatrue, jatrue);
    free_ini(admin->ini);

    return 0;
}
