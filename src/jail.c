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

JAIL *lookup_jail(JAILADMIN *admin, char *name)
{
    JAIL *jail;
    SQL_ROW *rows, *row;
    char buf[BUFSZ+1];

    memset(buf, 0x00, sizeof(buf));
    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_jails WHERE name = '%s'", admin->prefix, name);

    if (!(rows))
        return NULL;

    return NULL;
}
