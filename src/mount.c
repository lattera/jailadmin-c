#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"

#define BUFSZ 512

MOUNT **get_mounts(JAILADMIN *admin, JAIL *jail)
{
    MOUNT **mounts=NULL;
    SQL_ROW *rows, *row;
    char buf[BUFSZ+1];
    unsigned long i=0;

    memset(buf, 0x00, BUFSZ);
    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_mounts WHERE jail = '%s'", admin->prefix, jail->name);

    for (row = rows; row != NULL; row = row->next) {
        mounts = realloc(mounts, ++i * sizeof(MOUNT **));
        mounts[i-1] = xmalloc(sizeof(MOUNT));

        mounts[i-1]->target = strdup(get_column(row, "target"));
        mounts[i-1]->source = strdup(get_column(row, "source"));
        mounts[i-1]->driver = strdup(get_column(row, "driver"));
        mounts[i-1]->options = strdup(get_column(row, "options"));
    }

    sqldb_free_rows(rows);

    mounts = realloc(mounts, ++i * sizeof(MOUNT **));
    mounts[i-1] = NULL;

    return mounts;
}
