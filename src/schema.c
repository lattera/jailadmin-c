#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"

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

jabool is_schema_installed(JAILADMIN *admin)
{
    char buf[BUFSZ+1];
    SQL_ROW *rows;
    int num;

    rows = runsql(admin->ctx, "SELECT COUNT(*) AS TableCount FROM information_schema.tables WHERE table_schema = 'jails' AND table_name = 'jailadmin_jails'");
    sscanf(get_column(rows, "TableCount"), "%d", &num);

    sqldb_free_rows(rows);

    return (num > 0) ? jatrue : jafalse;
}

int install_schema(JAILADMIN *admin)
{
    char buf[BUFSZ+1];
    char *prefix;
    SECTION *db;

    if (is_schema_installed(admin))
        return 0;

    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, JAIL_SCHEMA, admin->prefix));
    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, BRIDGES_SCHEMA, admin->prefix));
    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, BRIDGE_PHYSICALS_SCHEMA, admin->prefix));
    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, SERVICES_SCHEMA, admin->prefix));
    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, EPAIRS_SCHEMA, admin->prefix));
    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, TEMPLATES_SCHEMA, admin->prefix));
    sqldb_free_rows(sqlfmt(admin->ctx, buf, BUFSZ, MOUNTS_SCHEMA, admin->prefix));

    return 0;
}
