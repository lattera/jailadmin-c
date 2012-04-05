#if !defined(_JAILADMIN_JAIL_H)
#define _JAILADMIN_JAIL_H

struct _network;

typedef struct _jail {
    char *name;
    char *dataset;
    char *path;
    char *route;

    char **services;

    struct _network_device **networks;
    MOUNT **mounts;
} JAIL;

JAIL *get_jail(JAILADMIN *, char *);
bool is_jail_online(JAIL *);
void print_jail(JAIL *);
bool start_jail(JAILADMIN *, JAIL *);
bool stop_jail(JAILADMIN *, JAIL *);

#endif
