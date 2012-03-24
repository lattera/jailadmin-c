#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"
#include "linkedlist.h"
#include "ini.h"
#include "misc.h"
#include "sql.h"
#include "schema.h"

#define BUFSZ 256

#define JAIL_SCHEMA "CREATE TABLE %sjailadmin_jails ( name VARCHAR(100) NOT NULL" \
    ", dataset VARCHAR(500) NOT NULL" \
    ", route VARCHAR(45)" \
    ");"

bool is_schema_installed(INI *ini, SQL_CTX *ctx)
{
    char buf[BUFSZ+1];
    SQL_ROW *rows;
    bool ret;
    int num;

    rows = runsql(ctx, "SELECT COUNT(*) AS TableCount FROM information_schema.tables WHERE table_schema = 'jails' AND table_name = 'jailadmin_jails'");
    sscanf(get_column(rows, "TableCount"), "%d", &num);

    sqldb_free_rows(rows);

    return (num > 0) ? true : false;
}

int install_schema(INI *ini, SQL_CTX *ctx)
{
    char buf[BUFSZ+1];
    char *prefix;
    SECTION *db;

    if (is_schema_installed(ini, ctx))
        return 0;

    db = get_section(ini, "db");
    prefix = get_section_var(db, "prefix");
    if (!(prefix))
        prefix = "";

    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, JAIL_SCHEMA, prefix));

    return 0;
}
