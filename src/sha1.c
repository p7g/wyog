#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "sha1.h"

const char HEX_DIGITS[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static void write_word(char *buf, uint32_t word)
{
	int i;

	/* a hex digit can represent 4 bits */
	for (i = 0; i < 8; i += 1)
		buf[7 - i] = HEX_DIGITS[(word >> (i * 4)) & 0xF];
}

static uint32_t circular_left_shift(uint32_t word, int n)
{
	return (word << n) | (word >> (32 - n));
}

uint32_t *pad_message(const char *message, uint64_t bit_length,
		size_t *len_out)
{
	int i, bit;
	size_t num_blocks, data_len;
	uint32_t *data;

	/* leave space for padding */
	num_blocks = (bit_length + 65) / 512 + 1;
	data = calloc((data_len = num_blocks * 16), sizeof(uint32_t));
	if (len_out != NULL)
		*len_out = data_len;

	for (i = 0; i < bit_length / 8; i += 1) 
		data[i / 4] |= message[i] << ((3 - (i % 4)) * 8);

	i = bit_length / 32;
	bit = 31 - (bit_length % 32);
	assert(0 <= bit && bit < 32);

	/* zero rest of word */
	data[i] &= ~((1 << bit) - 1);
	/* append 1 */
	data[i] |= 1 << bit;
	/* store the bit length at the end of the block */
	data[data_len - 2] = bit_length >> 32;
	data[data_len - 1] = bit_length & 0xFFFFFFFF;

	return data;
}

static uint32_t f(int t, uint32_t B, uint32_t C, uint32_t D)
{
	if (0 <= t && t <= 19)
		return (B & C) | (~B & D);
	if (20 <= t && t <= 39)
		return B ^ C ^ D;
	if (40 <= t && t <= 59)
		return (B & C) | (B & D) | (C & D);
	if (60 <= t && t <= 79)
		return B ^ C ^ D;
	abort();
}

static uint32_t K(int t)
{
	if (0 <= t && t <= 19)
		return 0x5A827999;
	if (20 <= t && t <= 39)
		return 0x6ED9EBA1;
	if (40 <= t && t <= 59)
		return 0x8F1BBCDC;
	if (60 <= t && t <= 79)
		return 0xCA62C1D6;
	abort();
}

static void digest(uint32_t *_M, size_t message_len, uint32_t out[5])
{
#define M(N) (_M + (N) * 16)
	/* try not to explode the stack */
	static uint32_t A, B, C, D, E,
			H0, H1, H2, H3, H4;
	static uint32_t W[80];
	uint32_t TEMP;
	int i, j, t;
	size_t num_blocks;

	H0 = 0x67452301;
	H1 = 0xEFCDAB89;
	H2 = 0x98BADCFE;
	H3 = 0x10325476;
	H4 = 0xC3D2E1F0;

	assert(message_len % 16 == 0); /* the message should be padded */
	num_blocks = message_len / 16;

	for (i = 0; i < num_blocks; i += 1) {
		for (j = 0; j < 16; j += 1)
			W[j] = M(i)[j];
		for (t = 16; t <= 79; t += 1)
			W[t] = circular_left_shift(
					W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16],
					1);
		A = H0; B = H1; C = H2; D = H3; E = H4;
		for (t = 0; t <= 79; t += 1) {
			TEMP = circular_left_shift(A, 5) + f(t, B, C, D) + E +
				W[t] + K(t);
			E = D;
			D = C;
			C = circular_left_shift(B, 30);
			B = A;
			A = TEMP;
		}
		H0 += A; H1 += B; H2 += C; H3 += D; H4 += E;
	}

	out[0] = H0;
	out[1] = H1;
	out[2] = H2;
	out[3] = H3;
	out[4] = H4;
#undef M
}

void wyog_sha1(const char *message, size_t message_len, sha1_hash out)
{
	uint32_t *data;
	uint32_t hashed[5];
	size_t data_len;
	int i;

	data = pad_message(message, message_len * 8, &data_len);
	digest(data, data_len, hashed);
	free(data);

	for (i = 0; i < 5; i += 1)
		write_word(out + 8 * i, hashed[i]);
	out[40] = 0;
}
