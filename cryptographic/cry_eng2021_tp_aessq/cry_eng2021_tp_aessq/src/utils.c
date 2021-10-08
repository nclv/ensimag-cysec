#include <stdint.h>
#include <stdio.h>

void print_array(uint8_t array[16]) {
	for (int i = 0; i < 16; ++i) {
		printf("%x - ", array[i]);
	}
	printf("\n");
}

void print_array2d(uint8_t array[256][16]) {
	for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 16; ++j) {
            printf("%x - ", array[i][j]);
        }
	    printf("\n");
    }
	printf("\n");
}

void xor_array(uint8_t l[16], uint8_t r[16], uint8_t result[16]) {
	for (size_t i = 0; i < 16; i++) {
		result[i] = l[i] ^ r[i];
	}
}