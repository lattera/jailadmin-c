#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"
#include "misc.h"
#include "linkedlist.h"
#include "sql.h"
#include "ini.h"
#include "schema.h"

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
    INI *ini;
    char *inifile;
    SQL_CTX *sql_ctx;
    SECTION *db_section;

    inifile = getvar("JAILADMIN_CONFIG", argc, argv, 1);

    ini = parse_ini(inifile);
    if (!(ini)) {
        fprintf(stderr, "[-] ini is null!\n");
        return 0;
    }

    db_section = get_section(ini, "db");
    sql_ctx = init_sql(get_section_var(db_section, "host"),
                       get_section_var(db_section, "user"),
                       get_section_var(db_section, "password"),
                       get_section_var(db_section, "db")
              );

    install_schema(ini, sql_ctx);

    close_sql(sql_ctx, true, true);
    free_ini(ini);

    return 0;
}
