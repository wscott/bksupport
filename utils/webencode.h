#ifndef _UTILS_WEBENCODE_H
#define _UTILS_WEBENCODE_H

#include <stdio.h>
#include "style.h"

void	webencode(FILE *out, u8 *ptr, int len);
char	*webdecode(char *data, char **buf, int *sizep);

#endif
