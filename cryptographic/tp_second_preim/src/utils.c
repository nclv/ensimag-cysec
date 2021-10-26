#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"
#include "xoshiro256starstar.h"

int assert_equals(uint32_t result[2], uint32_t expected[2]) {
    for (size_t i = 0; i < 2; ++i) {
        if (result[i] != expected[i]) {
            return -1;
        }
    }
    return 0;
}

void print_array(uint32_t *array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%x - ", array[i]);
    }
    printf("\n");
}

/*
 * Set the message @m to a random message.
 */
int random_m(uint32_t m[4]) {
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	read(fd, m, 16);
	close(fd);

	return 0;
}

/*
 * Set the message @m to a random message.
 */
void random_message(uint32_t m[4]) {
    
	uint64_t m01 = xoshiro256starstar_random();
	uint64_t m23 = xoshiro256starstar_random();
    m[0] = (uint32_t) m01 & 0xFFFFFF;
    m[1] = (uint32_t) (m01 >> 24) & 0xFFFFFF;
    m[0] = (uint32_t) m23 & 0xFFFFFF;
    m[3] = (uint32_t) (m23 >> 24) & 0xFFFFFF;
}