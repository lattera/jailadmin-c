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
        devices[i-1]->device = get_column(row, "device");
        devices[i-1]->ip = get_column(row, "ip");
    }

    devices = realloc(devices, ++i * sizeof(NETWORK_DEVICE **));
    devices[i-1] = NULL;

    sqldb_free_rows(rows);

    return devices;
}
