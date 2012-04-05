#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"

#define BUFSZ 512

NETWORK_DEVICE **get_devices(JAILADMIN *admin, JAIL *jail)
{
    char buf[BUFSZ+1];
    SQL_ROW *rows, *row;
    NETWORK_DEVICE **devices=NULL;
    unsigned long i=0;

    memset(buf, 0x00, BUFSZ);

    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_epairs WHERE jail = '%s'", admin->prefix, jail->name);
    if (!(rows))
        return NULL;

    for (row = rows; row != NULL; row = row->next) {
        devices = realloc(devices, ++i * sizeof(NETWORK_DEVICE **));
        if (!(devices))
            return NULL;

        devices[i-1] = xmalloc(sizeof(NETWORK_DEVICE));
        devices[i-1]->device = strdup(get_column(row, "device"));
        devices[i-1]->ip = strdup(get_column(row, "ip"));
        devices[i-1]->network = get_network(admin, get_column(row, "bridge"));
    }

    devices = realloc(devices, ++i * sizeof(NETWORK_DEVICE **));
    devices[i-1] = NULL;

    sqldb_free_rows(rows);

    return devices;
}

NETWORK *get_network(JAILADMIN *admin, char *name)
{
    char buf[BUFSZ+1];
    SQL_ROW *rows, *row;
    NETWORK *network;
    unsigned long i=0;
    
    memset(buf, 0x00, BUFSZ);

    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_bridges WHERE name = '%s'", admin->prefix, name);
    if (!(rows))
        return NULL;

    network = xmalloc(sizeof(NETWORK));
    network->name = strdup(get_column(rows, "name"));
    network->device = strdup(get_column(rows, "device"));
    network->ip = strdup(get_column(rows, "ip"));

    sqldb_free_rows(rows);

    rows = sqlfmt(admin->ctx, buf, BUFSZ, "SELECT * FROM %sjailadmin_bridge_physicals WHERE bridge = '%s'", name);
    for (row = rows; row != NULL; row = row->next) {
        network->physicals = realloc(network->physicals, ++i * sizeof(char **));
        network->physicals[i-1] = strdup(get_column(row, "device"));
    }

    network->physicals = realloc(network->physicals, ++i * sizeof(char **));
    network->physicals[i-1] = NULL;

    return network;
}
