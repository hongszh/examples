#include <stdlib.h>
#include "utils.h"

void print_array(int *array, int len) {
    for (size_t n = 0; n < len; n++)
    {
        printf("%d, ", array[n]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int array[] = {1, 4, 8, 3, 6, -1, 10, 0, -11, 11, -8, -5, 9};

    int len = sizeof(array) / sizeof(array[0]);
    insert_sort(array, len);

    return 0;
}
