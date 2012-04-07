#if !defined(_JAILADMIN_H)
#define _JAILADMIN_H

typedef enum _jailadmin_bool { jafalse=0, jatrue=1 } jabool;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "misc.h"
#include "sql.h"
#include "linkedlist.h"
#include "ini.h"

typedef struct _jailadmin {
    SQL_CTX *ctx;
    INI *ini;
    SECTION *db;
    char *prefix;
    WINDOW **windows;

    jabool interactive;
} JAILADMIN;

#include "jailadmin_mount.h"
#include "schema.h"
#include "jailadmin_jail.h"
#include "jailadmin_network.h"
#include "jailadmin_interactive.h"

#define SUDO(x) do { x = (!getuid() || !geteuid()) ? "" : "/usr/local/bin/sudo"; } while (0)

#endif
