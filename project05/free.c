#include <stdio.h>
#include "free.h"
#include "block.h"


int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < BYTE; i++)
        if (!(x & (1 << i)))
            return i;
    
    return FAILURE;
}

int calculate_block_number(int index, int bit_num)
{
	return (index * BYTE) + bit_num;
}

void set_free(unsigned char *block, int num, int set)
{
	int byte_num = num / BYTE;
	int bit_num = num % BYTE;
	if (set == IN_USE) {
		block[byte_num] |= set << bit_num;
	} else {
		block[byte_num] &= ~(1 << bit_num);
	}

}

int find_free(unsigned char *block)
{
	// Loop over each byte in the block
	for (int i = 0; i < BLOCK_SIZE; i ++) {
		unsigned char x = block[i];

		// Find a clear bit
		int bit_num = find_low_clear_bit(x);

		// If search succeeds, return the bit number
		if (bit_num != FAILURE) {
			return calculate_block_number(i, bit_num);
		}
	}
	return FAILURE;
}
