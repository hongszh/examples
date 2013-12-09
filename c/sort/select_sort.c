
#include <stdlib.h>
#include "utils.h"

int select_sort(int *array, int len) {
    if (array == NULL || len < 2) return;
    
    int min = 0;
    int index = 0;
    int swap  = 0;

    for (size_t i = 0; i < len; i++) {
         min = array[i];
        for (size_t j = i+1; j < len - 1; j++) {
            if (array[j] < min) {
                index = j;
                min = array[j];
            }
        }

        if (index > 0) {
            swap = array[i];
            array[i] = array[index];
            array[index] = swap;
        }

        printf("i=%d: ", i); print_array(array, len);
    }
}