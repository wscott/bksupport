#include "system.h"

/*
 * Return true if this character should be encoded according to RFC1738
 */
private int
is_encoded(int c)
{
	static	u8	*binchars = 0;

	unless (binchars) {
		int	i;
		char	*p;

		binchars = malloc(256);
		/* all encoded by default */
		for (i = 0; i < 256; i++) binchars[i] = 1;

		/* these don't need encoding */
		for (i = 'A'; i <= 'Z'; i++) binchars[i] = 0;
		for (i = 'a'; i <= 'z'; i++) binchars[i] = 0;
		for (i = '0'; i <= '9'; i++) binchars[i] = 0;
		for (p = "-_.~/@"; *p; p++) binchars[(int)*p] = 0;
	}
	return (binchars[c]);
}


/*
 * Encode the data in ptr/len and add it to buf
 * using data_append().
 */
char **
webencode(char **buf, u8 *ptr, int len)
{
	char	hex[4];

	while (len > 0) {
		/* suppress trailing null (common) */
		if ((len == 1) && !*ptr) break;

		if (*ptr == ' ') {
			hex[0] = '+';
			buf = str_nappend(buf, hex, 1, 0);
		} else if (is_encoded(*ptr)) {
			sprintf(hex, "%%%02x", *ptr);
			buf = str_nappend(buf, hex, 3, 0);
		} else {
			buf = str_nappend(buf, ptr, 1, 0);
		}
		++ptr;
		--len;
	}
	/* %FF(captials) is a special bk marker for no trailing null */
	if (len == 0) buf = str_nappend(buf, "%FF", 3, 0);
	return (buf);
}

/*
 * unpack a wrapped string from *data and put it in the buffer buf.
 * The string ends on the first '&' '=' or '\0'.
 * If successful, returns new pointer to data and sets size.
 * Else return 0.
 * Any whitespace in the string a ignored and skipped.
 */
char *
webdecode(char *data, char **buf, int *sizep)
{
	char	*p = data;
	char	*t;
	char	*ret;
	int	c;
	int	bin = 0;

	assert(buf);
	ret = t = malloc(strcspn(data, "=&") + 1);
	while (1) {
		switch (*p) {
		    case '+':
			*t++ = ' ';
			break;
		    case '%':
			if ((p[1] == 'F') && (p[2] == 'F')) {
				bin = 1;
				p += 2;
				break;
			}
			unless (sscanf(p+1, "%2x", &c) == 1) goto err;
			*t++ = c;
			p += 2;
			break;
		    case ' ': case '\n': case '\r': case '\t':
			break;
		    case '&': case '=': case 0:
			unless (bin) *t++ = 0; /* add trailing null */
			if (sizep) *sizep = (t - ret);
			*buf = ret;
			return (p);
		    default:
			*t++ = *p;
			break;
		}
		p++;
	}
err:
	fprintf(stderr, "ERROR: can't decode %s\n", p);
	return (0);
}

