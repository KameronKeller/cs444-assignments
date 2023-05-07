#include <stdio.h>
#include <string.h>
#include "ctest.h"
#include "image.h"
#include "block.h"

// # create_image_file()

// # read_block()

// # write_block()

// # locate_free_block()

// # toggle_free()

// # locate_free_inode()

#ifdef CTEST_ENABLE

void test_image_open_and_close(void)
{
	int image_fd = image_open("test", 0);
	CTEST_ASSERT(image_fd == 3, "Test create and open new image");
	CTEST_ASSERT(image_close() == 0, "Test closing and image file");

	image_fd = image_open("test", 1);
	CTEST_ASSERT(image_fd == 3, "Test create and truncate new image");
	CTEST_ASSERT(image_close() == 0, "Test closing truncated file");
}

void test_block_write_and_read(void)
{
	int block_num = 7;
	unsigned char test_data[BLOCK_SIZE] = {0}; // Initialize array block with zeros
	image_open("test_image", 0); // Open the file
	bwrite(block_num, test_data);

	unsigned char read_data[BLOCK_SIZE];
	bread(block_num, read_data);

	CTEST_ASSERT(memcmp(test_data, read_data, 3) == 0, "Test bwrite and bread");
	image_close();


}

int main(void)
{
    CTEST_VERBOSE(1);

	test_image_open_and_close();
	test_block_write_and_read();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("Running normally!\n");
}

#endif
