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

JAIL **get_jails(JAILADMIN *admin)
{
    JAIL **jails=NULL;
    SQL_ROW *rows, *row;
    unsigned int i=0;
    char buf[BUFSZ+1];

    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT name FROM %sjailadmin_jails", admin->prefix);
    for (row = rows; row != NULL; row = row->next) {
        jails = realloc(jails, ++i * sizeof(JAIL **));
        jails[i-1] = get_jail(admin, get_column(row, "name"));
    }

    sqldb_free_rows(rows);

    jails = realloc(jails, ++i * sizeof(JAIL **));
    jails[i-1] = NULL;

    return jails;
}

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
    jail->mounts = get_mounts(admin, jail);

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

jabool is_jail_fully_online(JAIL *jail)
{
    unsigned long i;
    jabool ret;

    ret = is_jail_online(jail);
    if (ret == jafalse)
        return jafalse;

    for (i=0; jail->networks[i] != NULL; i++)
        if (is_network_online(jail->networks[i]->network) == jafalse || is_network_device_online(jail->networks[i]) == jafalse)
            return jafalse;

    return jatrue;
}

jabool is_jail_online(JAIL *jail)
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

    return (strlen(buf) > 0) ? jatrue : jafalse;
}

jabool start_jail(JAILADMIN *admin, JAIL *jail)
{
    char *sudo;
    char buf[BUFSZ+1];
    unsigned long i;

    memset(buf, 0x00, BUFSZ);
    SUDO(sudo);

    if (is_jail_online(jail))
        return jatrue;

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

    for (i=0; jail->mounts[i] != NULL; i++) {
        snprintf(buf, BUFSZ, "%s /sbin/mount ", sudo);
        if (strlen(jail->mounts[i]->driver))
            snprintf(buf+strlen(buf), BUFSZ-strlen(buf), "-t '%s' ", jail->mounts[i]->driver);
        if (strlen(jail->mounts[i]->options))
            snprintf(buf+strlen(buf), BUFSZ-strlen(buf), "-o %s ", jail->mounts[i]->options);

        snprintf(buf+strlen(buf), BUFSZ-strlen(buf), "'%s' '%s/%s'", jail->mounts[i]->source, jail->path, jail->mounts[i]->target);
        system(buf);
    }

    for (i=0; jail->services[i] != NULL; i++) {
        snprintf(buf, BUFSZ, "%s /usr/sbin/jexec '%s' %s start", sudo, jail->name, jail->services[i]);
        system(buf);
    }

    return jatrue;
}

jabool stop_jail(JAILADMIN *admin, JAIL *jail)
{
    char *sudo;
    char buf[BUFSZ+1];
    unsigned int i;

    if (is_jail_online(jail) == jafalse)
        return jatrue;

    memset(buf, 0x00, BUFSZ);
    SUDO(sudo);

    snprintf(buf, BUFSZ, "%s /usr/sbin/jail -r '%s'", sudo, jail->name);
    system(buf);

    snprintf(buf, BUFSZ, "%s /sbin/umount '%s/dev'", sudo, jail->path);
    system(buf);

    for (i=0; jail->mounts[i] != NULL; i++) {
        snprintf(buf, BUFSZ, "%s /sbin/umount -f '%s/%s'", sudo, jail->path, jail->mounts[i]->target);
        system(buf);
    }

    for (i=0; jail->networks[i] != NULL; i++)
        bring_guest_offline(admin, jail, jail->networks[i]);

    return jatrue;
}

void print_jail(JAIL *jail)
{
    unsigned long i;

    fprintf(stderr, "[%s] online => %s\n", jail->name, (is_jail_online(jail)) ? "jatrue" : "jafalse");
    fprintf(stderr, "[%s] dataset => %s\n", jail->name, jail->dataset);
    fprintf(stderr, "[%s] route => %s\n", jail->name, jail->route);

    for (i=0; jail->networks[i] != NULL; i++) {
        fprintf(stderr, "[%s][net:%s] ip => %s\n", jail->name, jail->networks[i]->device, jail->networks[i]->ip);
    }
}
