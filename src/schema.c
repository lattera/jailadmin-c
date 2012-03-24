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

#define BRIDGES_SCHEMA "CREATE TABLE %sjailadmin_bridges ( name VARCHAR(100) NOT NULL" \
    ", device VARCHAR(20) NOT NULL" \
    ", ip VARCHAR(45)" \
    ");"

#define BRIDGE_PHYSICALS_SCHEMA "CREATE TABLE %sjailadmin_bridge_physicals ( bridge VARCHAR(100) NOT NULL" \
    ", device VARCHAR(20) NOT NULL" \
    ");"

#define SERVICES_SCHEMA "CREATE TABLE %sjailadmin_services ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY" \
    ", path VARCHAR(500) NOT NULL" \
    ", jail VARCHAR(100) NOT NULL" \
    ");"

#define EPAIRS_SCHEMA "CREATE TABLE %sjailadmin_epairs ( device VARCHAR(20) NOT NULL" \
    ", jail VARCHAR(100) NOT NULL" \
    ", bridge VARCHAR(100) NOT NULL" \
    ", ip VARCHAR(45) NOT NULL" \
    ");"

#define TEMPLATES_SCHEMA "CREATE TABLE %sjailadmin_templates ( name VARCHAR(100) NOT NULL" \
    ", snapshot VARCHAR(500) NOT NULL" \
    ");"

#define MOUNTS_SCHEMA "CREATE TABLE %sjailadmin_mounts ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY" \
    ", jail VARCHAR(100) NOT NULL" \
    ", target VARCHAR(500) NOT NULL" \
    ", source VARCHAR(500) NOT NULL" \
    ", driver VARCHAR(50)" \
    ", options VARCHAR(200)" \
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
    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, BRIDGES_SCHEMA, prefix));
    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, BRIDGE_PHYSICALS_SCHEMA, prefix));
    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, SERVICES_SCHEMA, prefix));
    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, EPAIRS_SCHEMA, prefix));
    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, TEMPLATES_SCHEMA, prefix));
    sqldb_free_rows(sqlfmt(ctx, buf, BUFSZ, MOUNTS_SCHEMA, prefix));

    return 0;
}
