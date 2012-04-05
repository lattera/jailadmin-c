#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"
#include "misc.h"
#include "linkedlist.h"
#include "sql.h"
#include "ini.h"

#define BUFSZ 512

JAIL *get_jail(JAILADMIN *admin, char *name)
{
    JAIL *jail;
    SQL_ROW *rows, *row;
    char buf[BUFSZ+1];

    memset(buf, 0x00, sizeof(buf));
    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_jails WHERE name = '%s'", admin->prefix, name);
    if (!(rows))
        return NULL;

    jail = xmalloc(sizeof(JAIL));
    jail->name = strdup(get_column(rows, "name"));
    jail->route = strdup(get_column(rows, "route"));
    jail->dataset = strdup(get_column(rows, "dataset"));
    jail->networks = get_devices(admin, jail);

    sqldb_free_rows(rows);

    return jail;
}

void print_jail(JAIL *jail)
{
    unsigned long i;

    fprintf(stderr, "[%s] dataset => %s\n", jail->name, jail->dataset);
    fprintf(stderr, "[%s] route => %s\n", jail->name, jail->route);

    for (i=0; jail->networks[i] != NULL; i++) {
        fprintf(stderr, "[%s][net:%s] ip => %s\n", jail->name, jail->networks[i]->device, jail->networks[i]->ip);
    }
}
