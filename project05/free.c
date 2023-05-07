#include <stdio.h>
#include "free.h"
#include "block.h"

# define BYTE 8

int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;
    
    return -1;
}

int calculate_block_number(int index, int bit_num)
{
	return (index * BYTE) + bit_num;
}

void set_free(unsigned char *block, int num, int set)
{
	int byte_num = num / 8;  // 8 bits per byte
	int bit_num = num % 8;
	if (set == 1) {
		block[byte_num] |= set << bit_num;
	} else {
		block[byte_num] &= ~(1 << bit_num);
	}

}

int find_free(unsigned char *block)
{
	// int size = sizeof(block) / sizeof(block[0]);
	// printf("size: %d\n", size);
	for (int i = 0; i < BLOCK_SIZE; i ++) {
		unsigned char x = block[i];

		int bit_num = find_low_clear_bit(x);
			// printf("byte: %d\n", i);
			// printf("\tbit_num: %d\n", bit_num);
		if (bit_num != -1) {

			return calculate_block_number(i, bit_num);
		}
	}
	return 0;
}
