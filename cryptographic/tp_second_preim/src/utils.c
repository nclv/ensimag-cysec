#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "xoshiro256starstar.h"

/**
 * @brief assert that the 2 arrays are equals
 *
 * @param actual
 * @param expected
 * @param actual_size
 * @param expected_size
 * @return int, 1 if equals, 0 otherwise
 */
int assert_equals(const void *actual, const void *expected, size_t actual_size,
				  size_t expected_size) {

	return actual_size == expected_size &&
		   !memcmp(actual, expected, actual_size);
}

/**
 * @brief print the content of @array
 *
 * @param array
 * @param size
 */
void print_array(const uint32_t *array, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		printf("%x - ", array[i]);
	}
	printf("\n");
}

/**
 * @brief Initialize @m with a random message
 *
 * @param m
 * @return int
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

/**
 * @brief Initialize m with a random message
 *
 * @param m
 */
void random_message(uint32_t m[4]) {
	uint64_t m01 = xoshiro256starstar_random();
	uint64_t m23 = xoshiro256starstar_random();
	m[0] = (uint32_t)m01 & 0xFFFFFF;
	m[1] = (uint32_t)(m01 >> 24) & 0xFFFFFF;
	m[2] = (uint32_t)m23 & 0xFFFFFF;
	m[3] = (uint32_t)(m23 >> 24) & 0xFFFFFF;
}