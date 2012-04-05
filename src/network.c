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

bool is_network_online(NETWORK *network)
{
    char buf[BUFSZ+1];
    FILE *fp;

    memset(buf, 0x00, BUFSZ);
    snprintf(buf, BUFSZ, "/sbin/ifconfig '%s' 2>&1 | grep -v 'does not exist'", network->device);
    
    fp = popen(buf, "r");
    memset(buf, 0x00, BUFSZ);
    if ((fp)) {
        readf(buf, BUFSZ, fp);
        pclose(fp);
    }

    return (strlen(buf) > 0) ? true : false;
}

bool bring_network_online(JAILADMIN *admin, NETWORK *network)
{
    char *sudo;
    char buf[BUFSZ+1];
    unsigned long i;

    if (is_network_online(network))
        return true;

    memset(buf, 0x00, BUFSZ);
    SUDO(sudo);

    snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%s' create 2>&1", sudo, network->device);
    system(buf);

    snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%s' up 2>&1", sudo, network->device);
    system(buf);

    if (strlen(network->ip)) {
        snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%s' '%s' 2>&1", sudo, network->device, network->ip);
        system(buf);
    }

    for (i=0; network->physicals[i] != NULL; i++) {
        snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%s' addm '%s' 2>&1", sudo, network->device, network->physicals[i]);
        system(buf);
    }

    return true;
}

bool is_network_device_online(NETWORK_DEVICE *device)
{
    char buf[BUFSZ+1];
    FILE *fp;

    memset(buf, 0x00, BUFSZ);
    snprintf(buf, BUFSZ, "/sbin/ifconfig '%sa' 2>&1 | grep -v 'does not exist'", device->device);

    fp = popen(buf, "r");
    memset(buf, 0x00, BUFSZ);
    if ((fp)) {
        readf(buf, BUFSZ, fp);
        pclose(fp);
    }

    return (strlen(buf) > 0) ? true : false;
}

bool bring_host_online(JAILADMIN *admin, JAIL *jail, NETWORK_DEVICE *device)
{
    char *sudo;
    char buf[BUFSZ+1];

    if (is_network_device_online(device) == true)
        return true;

    if (is_jail_online(jail) == false)
        return false;

    if (bring_network_online(admin, device->network) == false)
        return false;

    memset(buf, 0x00, BUFSZ);
    SUDO(sudo);

    snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%s' create 2>&1", sudo, device->device);
    system(buf);

    snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%s' addm '%sa' 2>&1", sudo, device->network->device, device->device);
    system(buf);

    snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%sa' up 2>&1", sudo, device->device);
    system(buf);

    return true;
}

bool bring_guest_online(JAILADMIN *admin, JAIL *jail, NETWORK_DEVICE *device)
{
    char *sudo;
    char buf[BUFSZ+1];

    if (bring_host_online(admin, jail, device) == false)
        return false;

    memset(buf, 0x00, BUFSZ);
    SUDO(sudo);

    snprintf(buf, BUFSZ, "%s /sbin/ifconfig '%sb' vnet '%s' 2>&1", sudo, device->device, jail->name);
    system(buf);

    snprintf(buf, BUFSZ, "%s /usr/sbin/jexec '%s' ifconfig '%sb' '%s' 2>&1", sudo, jail->name, device->device, device->ip);
    system(buf);

    return true;
}
