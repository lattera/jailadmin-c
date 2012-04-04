#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"

void usage(char *name)
{
    fprintf(stderr, "USAGE: %s [ini]\n", name);
    fprintf(stderr, "Environment variables that can be set:\n");
    fprintf(stderr, "JAILADMIN_CONFIG\t- INI file for config.\n");
    fprintf(stderr, "    If set, do not pass file name as an argument to the program\n");
    exit(1);
}

char *getvar(char *envname, int argc, char *argv[], int argi)
{
    char *p;

    p = getenv(envname);
    if ((p))
        return p;

    if (argc <= argi)
        usage(argv[0]);

    return argv[argi];
}

int main(int argc, char *argv[])
{
    JAILADMIN *admin;
    char *inifile;

    admin = xmalloc(sizeof(JAILADMIN));

    inifile = getvar("JAILADMIN_CONFIG", argc, argv, 1);

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

    admin->prefix = get_section_var(admin->db, "prefix");
    if (!(admin->prefix))
        admin->prefix = "";

    install_schema(admin);

    close_sql(admin->ctx, true, true);
    free_ini(admin->ini);

    return 0;
}
