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

    for (section = ini->sections; section != NULL; section = section->next) {
        fprintf(stderr, "[%s]\n", section->name);

        if ((section->settings)) {
            for (setting = section->settings->head; setting != NULL; setting = setting->next) {
                fprintf(stderr, "    %s=%s\n", setting->name, setting->data);
            }
        }
    }

    return 0;
}
