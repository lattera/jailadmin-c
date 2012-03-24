#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"
#include "misc.h"
#include "linkedlist.h"
#include "sql.h"
#include "ini.h"

int main(int argc, char *argv[])
{
    INI *ini;
    SECTION *section;
    LISTNODE *setting;

    ini = parse_ini(argv[1]);
    if (!(ini)) {
        fprintf(stderr, "[-] ini is null!\n");
        return 0;
    }

    printf("%s\n", get_section_var(get_section(ini, "db"), "user"));

    free_ini(ini);

    return 0;
}
