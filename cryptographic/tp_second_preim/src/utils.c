#include "utils.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int assert_equals(uint32_t result[2], uint32_t expected[2]) {
    for (size_t i = 0; i < 2; ++i) {
        if (result[i] != expected[i]) {
            return -1;
        }
    }
    return 0;
}

void print_array(uint32_t array[2]) {
    for (size_t i = 0; i < 2; ++i) {
        printf("%x - ", array[i]);
    }
    printf("\n");
}