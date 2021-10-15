#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

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

/* 
 * Set @byte to a random byte.
*/
int random_byte(uint8_t *byte) {
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	read(fd, byte, 1);
	close(fd);

	return 0;
}


/* 
 * Set @key to a random byte array.
*/
int random_key(uint8_t key[16]) {
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	read(fd, key, 8);
	close(fd);

	return 0;
}