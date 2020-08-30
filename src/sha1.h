#ifndef WYOG_SHA1_H
#define WYOG_SHA1_H

#include <stddef.h>

typedef char sha1_hash[41];

void wyog_sha1(const char *message, size_t message_len, sha1_hash out);

#endif
