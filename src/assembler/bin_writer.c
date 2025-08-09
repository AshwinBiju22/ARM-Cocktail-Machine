#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void bin_writer(uint32_t *data, size_t size, char *filename) {
	FILE *out = fopen(filename, "wb");
	if (!out) {
		printf("ERROR: File open to create %s", filename);
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < size; ++i) {
		uint32_t value = data[i];
		uint8_t bytes[4] = {value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF,
							(value >> 24) & 0xFF};
		if (fwrite(bytes, 1, 4, out) != 4) { // f..k little endian man, what even is the benefit
			printf("ERROR: Error writing %s", filename);
			fclose(out);
			exit(EXIT_FAILURE);
		}
	}

	fclose(out);
}
