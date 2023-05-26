#include <unistd.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "mkfs.h"


void mkfs(void)
{
	unsigned char initialize_data[FOUR_MB_IMAGE];
	memset(initialize_data, 0, FOUR_MB_IMAGE);

	write(image_fd, initialize_data, FOUR_MB_IMAGE);

	for (int i = 0; i < METADATA; i++) {
		alloc();
	}

}
