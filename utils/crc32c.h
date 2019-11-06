#ifndef _UTILS_CRC32C_H
#define _UTILS_CRC32C_H

#include <stdlib.h>

#include "style.h"

u32	crc32c(u32 crc, const void *chunk, size_t len);

#endif
