
#include <stdlib.h>
#include "utils.h"

int insert_sort(int *array, int len) {
    if (array == NULL || len < 2) return -1;

    int i = 0, j = 0;
    for (i = 1; i < len; i++) {
#if 1   
        int key = array[i];
        for (j = i - 1; j >= 0 && array[j] > key; j--) {
            array[j + 1] = array[j];
        }
        array[j + 1] = key;
#else
        int *a = array;
        int t = a[i];
        j = i - 1;
        while(j >= 0){
            if(a[j] > t) {
                a[j + 1] = a[j];
            }
            else
                break;
            j--;
        }
        j += 1;
        a[j] = t;
#endif
        
        printf("i=%d, j=%d : ", i, j); print_array(array, len);
    }
    return 0;
}