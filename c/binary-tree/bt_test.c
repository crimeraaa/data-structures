#include <stdlib.h>
#include <stdio.h>

#include "genbinarytree.h"

#define ARRAYLENGTH(x) (sizeof(x) / sizeof(x[0]))

void btree_print(void *obj)
{
    printf("%i\n", *(int*)obj);
}

// lmao
int testarray[] = 
{
                                      13, 
    //               |----------------|----------------|
                     5,                                19, 
    //      |--------|--------|               |--------|--------|
            2,                9,              15,              23, 
    // |----|----|       |----|----|     |----|----|       |----|----|
                 4,      7,              14,      17,      21,      27, 
    //        |--|--| |--|--|         |--|--|   |--|--|  |--|--|   |--|--| 
                      6,    8,        11,       16,  18, 20,  22,  25,  29
};

const int testlen = ARRAYLENGTH(testarray);

int main(void)
{
    bt_root *btree = bt_init(NULL, btree_print, NULL);
    if (!btree) return EXIT_FAILURE;

    for (int i = 0; i < testlen; i++)
    {
        int *ptr = malloc(sizeof(int));
        if (!ptr)
        {
            bt_destroy(&btree);
            return EXIT_FAILURE;
        }
        *ptr = testarray[i];
        bt_insert(btree, ptr);
    }
    printf("<< TREE PRINTOUT 1 >>\n");
    bt_printbt(btree);
    printf("\n");

    bt_destroy(&btree);

    return EXIT_SUCCESS;
}