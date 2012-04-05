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
NETWORK *get_network(JAILADMIN *, char *);
jabool is_network_online(NETWORK *);
jabool bring_network_online(JAILADMIN *, NETWORK *);
jabool is_network_device_online(NETWORK_DEVICE *);
jabool bring_host_online(JAILADMIN *, JAIL *, NETWORK_DEVICE *);
jabool bring_guest_online(JAILADMIN *, JAIL *, NETWORK_DEVICE *);
jabool bring_guest_offline(JAILADMIN *, JAIL *, NETWORK_DEVICE *);

#endif
