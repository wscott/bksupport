#include "system.h"

/*
 * Routines to save and restore a hash to and from a file. The format
 * used in the file is the same as in the bugdb.
 */

private	int binaryField(u8 *data, int len);
private	int goodkey(u8 *key, int len);
private	void savekey(hash *h, int base64, char *key, char **val);
private	void writeField(FILE *f, char *key, u8 *data, int len);

/* These are from tomcrypt... */
extern int base64_decode(const unsigned char *in,  unsigned long inlen,
    unsigned char *out, unsigned long *outlen);
extern int base64_encode(const unsigned char *in,  unsigned long inlen,
    unsigned char *out, unsigned long *outlen);

/*
 * write the hash files to a file named by path.
 * returns -1 on error, or 0
 */
int
hash_toFile(hash *h, char *path)
{
	FILE	*f;
	int	rc = -1;

	if (f = fopen(path, "w")) {
		rc = hash_toStream(h, f);
		fclose(f);
	}
	return (rc);
}

/*
 * write the hash files to a FILE*
 * returns -1 on error, or 0
 */
int
hash_toStream(hash *h, FILE *f)
{
	char	**fieldlist = 0;
	int	i, rc = -1;
	u8	*data;

	assert(h && f);

	/*
	 * Sort the fields and print them
	 */
	EACH_HASH(h) {
		unless (goodkey(h->kptr, h->klen)) goto out;
		fieldlist = addLine(fieldlist, h->kptr);
	}
	sortLines(fieldlist, 0);
	EACH(fieldlist) {
		data = hash_fetchStr(h, fieldlist[i]);
		writeField(f, fieldlist[i], data, h->vlen);
	}
	rc = 0;
out:	freeLines(fieldlist, 0);
	return (rc);
}

/*
 * Read a file written by the function above and add keys to 'h'
 * overwriting any existing keys.
 * If h==0 and path doesn't exist or is empty, then 0 is returned.
 */
hash *
hash_fromFile(hash *h, char *path)
{
	FILE	*f;

	if (f = fopen(path, "r")) {
		h = hash_fromStream(h, f);
		fclose(f);
	}
	return (h);
}

/*
 * Read a stream written by the function above and add keys to 'h'
 * overwriting any existing keys.
 * The function returns at EOF or when a line with just "@\n" is
 * encountered.  (record separator to put multiple KV's in a file)
 * If h==0, then a new hash is created if any data is found.
 */
hash *
hash_fromStream(hash *h, FILE *f)
{
	char	*line;
	char	*key = 0;
	int	base64 = 0;
	char	**val = 0;
	char	*p;
	unsigned long len;
	char	data[256];

	assert(f);
	while (line = fgetline(f)) {
		if ((line[0] == '@') && (line[1] != '@')) {
			unless (line[1]) break; /* @ == end of record */
			if (key || val) {
				/* save old key */
				unless (h) h = hash_new(HASH_MEMHASH);
				savekey(h, base64, key, val);
				key = 0;
				val = 0;
			}
			base64 =
			    ((p = strchr(line, ' ')) && streq(p, " base64"));
			if (p) {
				unless (base64) {
					fprintf(stderr,
					    "hash_fromFile: bad line '%s'\n",
					    line);
				}
				*p = 0;
			}
			key = strdup(line+1);
		} else {
			if (*line == '@') ++line; /* skip escaped @ */
			if (base64) {
				len = sizeof(data);
				base64_decode(line, strlen(line), data, &len);
				val = data_append(val, data, len, 0);
			} else {
				if (val) val = str_append(val, "\n", 0);
				val = str_append(val, line, 0);
			}
		}
	}
	if (key || val) {
		unless (h) h = hash_new(HASH_MEMHASH);
		savekey(h, base64, key, val);
	}
	return (h);
}


private int
goodkey(u8 *key, int len)
{
	int	i;

	for (i = 0; i < (len-1); i++) {
		unless (isprint(key[i]) && !isspace(key[i])) {
			fprintf(stderr, "%s invalid: %c\n", key, key[i]);
			return (0);
		}
	}
	unless (key[len-1] == 0) {
		fprintf(stderr, "%s, no null\n", key);
		return (0);
	}
	return (1);
}

/*
 * Search for fields that need to be encoded when doing a dbimplode.
 * Currently check for:
 *    Contains non-printable characters
 *    Has a line longer that 256 characters
 *    Is not null terminated
 */
private int
binaryField(u8 *data, int len)
{
	int	i;
	int	lastret = 0;

	for (i = 0; i < (len-1); i++) {
		int	c = data[i];
		unless (isprint(c) || isspace(c)) return (1);
		if (c == '\n') {
			if ((i - lastret) > 256) return (1);
			lastret = i;
		}
	}
	if ((data[len-1] != 0) || ((i - lastret) > 256)) return (1);
	return (0);
}

private void
writeField(FILE *f, char *key, u8 *data, int len)
{
	unsigned long	inlen, outlen;
	u8	*p;
	char	out[128];

	fputc('@', f);
	fputs(key, f);
	if (binaryField(data, len)) {
		fputs(" base64\n", f);
		while (len) {
			inlen = min(48, len);
			outlen = sizeof(out);
			if (base64_encode(data, inlen, out, &outlen)) {
				fprintf(stderr, "writeField: base64 err\n");
				exit(1);
			}
			fwrite(out, 1, outlen, f);
			fputc('\n', f);
			data += inlen;
			len -= inlen;
		}
	} else {
		/* data is normal \0 terminated C string */
		fputc('\n', f);
		while (*data) {
			if (*data == '@') fputc('@', f);
			p = data;
			while (*p && (*p++ != '\n'));
			fwrite(data, p - data, 1, f);
			data = p;
		}
	}
	fputc('\n', f);
}

private void
savekey(hash *h, int base64, char *key, char **val)
{
	u8	*data;
	int	len;

	unless (key) key = strdup("__HEADER__");
	if (base64) {
		data = data_pullup(&len, val);
	} else {
		data = str_pullup(&len, val);
		++len;
	}
	hash_store(h, key, strlen(key)+1, data, len); /* overwrite existing */
	free(key);
	free(data);
}

int
hashfile_test_main(int ac, char **av)
{
	int	i;
	hash	*h = 0;
	char	**keys = 0;
	char	*key, *val;
	FILE	*f;

	unless (ac == 3) {
usage:		fprintf(stderr, "usage: %s [r|w|n] <file>\n", av[0]);
		return (1);
	}
	switch (av[1][0]) {
	    case 'r':
		// read all hashes from av[2], dump to stdout
		unless (f = fopen(av[2], "r")) return (0);
		while (!feof(f)) {
			unless (h = hash_fromStream(0, f)) goto next;
			EACH_HASH(h) keys = addLine(keys, h->kptr);
			sortLines(keys, 0);
			EACH(keys) {
				val = hash_fetchStr(h, keys[i]);
				if (val[h->vlen-1]) {
					// val not null terminated
					printf("%s => ", keys[i]);
					fwrite(val, h->vlen, 1, stdout);
					printf("\n");
				} else {
					printf("%s => %s\n", keys[i], val);
				}
			}
			freeLines(keys, 0); keys = 0;
			hash_free(h); h = 0;
next:			unless (feof(f)) printf("---\n");
		}
		fclose(f);
		break;
	    case 'w':
		// read hash from av[2], write back out to stdout
		h = hash_fromFile(0, av[2]);
		hash_toStream(h, stdout);
		break;
	    case 'n':
		// create a hash with a non-null-terminated key, write to av[2]
		key = "nonterm";
		h = hash_new(HASH_MEMHASH);
		hash_insert(h, key, 7, "value", 6);
		hash_toFile(h, av[2]);
		break;
	    default: goto usage;
	}
	if (h) hash_free(h);
	return (0);
}
