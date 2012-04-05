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
    FILE *fp;
    char buf[BUFSZ+1];
    unsigned int i;

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

    snprintf(buf, BUFSZ, "/sbin/zfs get -H -o value mountpoint \"%s\"", jail->dataset);
    fp = popen(buf, "r");
    if ((fp)) {
        memset(buf, 0x00, BUFSZ);

        readf(buf, BUFSZ, fp);
        jail->path = strdup(buf);

        pclose(fp);
    }

    i=0;
    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_services WHERE jail = '%s'", admin->prefix, name);
    for (row = rows; row != NULL; row = row->next) {
        jail->services = realloc(jail->services, ++i * sizeof(char **));
        jail->services[i-1] = strdup(get_column(row, "path"));
    }

    jail->services = realloc(jail->services, ++i * sizeof(char **));
    jail->services[i-1] = NULL;

    return jail;
}

bool is_jail_online(JAIL *jail)
{
    FILE *fp;
    char buf[BUFSZ+1];

    memset(buf, 0x00, BUFSZ);
    snprintf(buf, BUFSZ, "/usr/sbin/jls -n -j \"%s\" jid 2>&1 | grep -v \"%s\"", jail->name, jail->name);

    fp = popen(buf, "r");
    memset(buf, 0x00, BUFSZ);

    if ((fp)) {
        readf(buf, BUFSZ, fp);
        pclose(fp);
    }

    return (strlen(buf) > 0) ? true : false;
}

bool start_jail(JAILADMIN *admin, JAIL *jail)
{
    char *sudo;
    char buf[BUFSZ+1];
    unsigned long i;

    memset(buf, 0x00, BUFSZ);
    SUDO(sudo);

    if (is_jail_online(jail))
        return true;

    snprintf(buf, BUFSZ, "%s /sbin/mount -t devfs devfs \"%s/dev\"", sudo, jail->path);
    system(buf);

    snprintf(buf, BUFSZ, "%s /usr/sbin/jail -c vnet 'name=%s' 'host.hostname=%s' 'path=%s' persist", sudo, jail->name, jail->name, jail->path);
    system(buf);

    for (i=0; jail->networks[i] != NULL; i++) {
        bring_host_online(admin, jail, jail->networks[i]);
        bring_guest_online(admin, jail, jail->networks[i]);
    }

    if (strlen(jail->route)) {
        snprintf(buf, BUFSZ, "%s /usr/sbin/jexec '%s' route add default '%s'", sudo, jail->name, jail->route);
        system(buf);
    }

    for (i=0; jail->services[i] != NULL; i++) {
        snprintf(buf, BUFSZ, "%s /usr/sbin/jexec '%s' %s start", sudo, jail->name, jail->services[i]);
        system(buf);
    }

    return true;
}

void print_jail(JAIL *jail)
{
    unsigned long i;

    fprintf(stderr, "[%s] online => %s\n", jail->name, (is_jail_online(jail)) ? "true" : "false");
    fprintf(stderr, "[%s] dataset => %s\n", jail->name, jail->dataset);
    fprintf(stderr, "[%s] route => %s\n", jail->name, jail->route);

    for (i=0; jail->networks[i] != NULL; i++) {
        fprintf(stderr, "[%s][net:%s] ip => %s\n", jail->name, jail->networks[i]->device, jail->networks[i]->ip);
    }
}
