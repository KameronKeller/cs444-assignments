#include <stdio.h>
#include "image.h"
#include "ctest.h"

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

int main(void)
{
    CTEST_VERBOSE(1);

	test_image_open_and_close();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("Running normally!\n");
}

#endif
