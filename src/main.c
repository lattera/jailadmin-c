#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"
#include "misc.h"
#include "linkedlist.h"
#include "sql.h"

int main(int argc, char *argv[])
{
    SQL_CTX *ctx;
    LINKEDLIST *list;

    list = xmalloc(sizeof(LIST));
    AddNode(list, "test 1", "data for test 1", strlen("data for test 1"));
    AddNode(list, "test 2", "data for test 2", strlen("data for test 2"));
    FreeList(list);

    return 0;
}
