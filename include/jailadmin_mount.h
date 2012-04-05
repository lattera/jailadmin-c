#if !defined(_JAILADMIN_MOUNT_H)
#define _JAILADMIN_MOUNT_H

struct _jail;

typedef struct _mount {
    char *target;
    char *source;
    char *driver;
    char *options;
} MOUNT;

MOUNT **get_mounts(JAILADMIN *, struct _jail *);

#endif
