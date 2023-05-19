#include <unistd.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "mkfs.h"

void initialize_file(unsigned char *block, int value, int size) {
	for (int i = 0; i < size; i++) {
		block[i] = value;
	}
}

void mkfs(void)
{
	unsigned char initialize_data[FOUR_MB_IMAGE];
	memset(initialize_data, 0, FOUR_MB_IMAGE);
	// initialize_file(initialize_data, ZEROS, FOUR_MB_IMAGE);

	write(image_fd, initialize_data, FOUR_MB_IMAGE);

	for (int i = 0; i < METADATA; i++) {
		alloc();
	}

}
