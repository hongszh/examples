#include <stdlib.h>

#include "utils.h"

int bubble_sort(int *arr, int len) {

    int i, j, swap;

    if (arr == NULL || len < 2) {
        return 0;
    }

    for (i = 0; i < len; i++) {
        printf("origin array: ");
        print_array(arr, len);
        
        for (j = 0; j < len-i-1; j++) {
            if (arr[j+1] > arr[j]) {
                swap = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = swap;
            }
            printf("i=%d, j=%d : ", i, j); print_array(arr, len);
        }

        /* print sorting array */
        printf("\nsorting array: i = %d:  ", i);
        print_array(arr, len);
        printf("\n------------------------------\n\n");
    }

    printf("\n\n sorted array: ");
    print_array(arr, len);
    printf("sort done!\n");
}