#if !defined(_JAILADMIN_JAIL_H)
#define _JAILADMIN_JAIL_H

struct _network;
struct _mount;

typedef struct _jail {
    char *name;
    char *dataset;
    char *route;

    char **services;

    struct _network_device **networks;
    struct _mount **mounts;
} JAIL;

JAIL *get_jail(JAILADMIN *, char *);
void print_jail(JAIL *);

#endif
