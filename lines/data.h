#ifndef _LIBC_LINES_DATA_H
#define _LIBC_LINES_DATA_H

typedef struct {
	char	*buf;		/* user's data */
	u32	len;		/* length of user's data */
	u32	size;		/* malloc'ed size */
} DATA;

void	data_setSize(DATA *d, u32 size);
void	data_resize(DATA *d, u32 newlen);
void	data_append(DATA *d, void *data, u32 len);
#define	data_appendStr(f, s)       data_append(f, (s), strlen(s))

#endif
