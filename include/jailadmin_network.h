#if !defined(_JAILADMIN_NETWORK_H)
#define _JAILADMIN_NETWORK_H

struct _jail;

typedef struct _network {
    char *name;
    char *device;
    char *ip;
    char **physicals;
} NETWORK;

typedef struct _network_device {
    NETWORK *network;
    char *device;
    char *ip;
} NETWORK_DEVICE;

NETWORK_DEVICE **get_devices(JAILADMIN *, struct _jail *);

#endif
