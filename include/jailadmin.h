#if !defined(_JAILADMIN_H)
#define _JAILADMIN_H

typedef enum _bool { false=0, true=1 } bool;

#include "misc.h"
#include "sql.h"
#include "linkedlist.h"
#include "ini.h"

typedef struct _jailadmin {
    SQL_CTX *ctx;
    INI *ini;
    SECTION *db;
    char *prefix;
} JAILADMIN;

#include "schema.h"
#include "jailadmin_jail.h"

#endif
