//
// Created by Jiawei Wang on 2/3/22.
//

#include "stdbool.h"
extern void svf_prin(int, char*);

int getValue(int* arr, int idx) {
    return arr[idx];
}

int main() {
    int arr[2];
    arr[0] = 0;
    arr[1] = 1;
    int v = getValue(arr, 1);
    svf_print(v, "value");
    svf_assert(v == 1);
    return 0;
}