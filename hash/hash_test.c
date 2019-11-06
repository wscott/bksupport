#include "hash/hash.h"
#include <unistd.h>
#include "lines/lines.h"
#include "utils/base64.h"
#include "utils/webencode.h"

int
main(int ac, char **av)
{
	int	i, c;
	hash	*h = 0;
	char	**keys = 0;
	char	*file;
	char	*key, *val;
	int	mode = 0;
	int	hex = 0;
	FILE	*f;

	while ((c = getopt(ac, av, "nrwX")) != -1) {
		switch (c) {
		    case 'n': mode = c; break;
		    case 'r': mode = c; break;
		    case 'w': mode = c; break;
		    case 'X': hex = 1; break;
		    default: goto usage;
		}
	}
	file = av[optind];
	unless (file && mode) {
usage:		fprintf(stderr, "usage: bk _hashfile_test [-nrwX] file\n");
		return (1);
	}
	switch (mode) {
	    case 'r':
		// read all hashes from av[2], dump to stdout
		unless (f = fopen(file, "r")) return (1);
		while (!feof(f)) {
			unless (h = hash_fromStream(0, f)) goto next;
			EACH_HASH(h) keys = addLine(keys, h->kptr);
			sortLines(keys, 0);
			EACH(keys) {
				printf("%s =>", keys[i]);
				val = hash_fetchStr(h, keys[i]);
				if (hex) {
					for (c = 0; c < h->vlen; c++) {
						printf(" %02x", val[c]);
					}
				} else {
					printf(" %s", val);
				}
				printf("\n");
			}
			freeLines(keys, 0); keys = 0;
			hash_free(h); h = 0;
next:			unless (feof(f)) printf("---\n");
		}
		fclose(f);
		break;
	    case 'w':
		// read hash from av[2], write back out to stdout
		h = hash_fromFile(0, file);
		hash_toStream(h, stdout);
		break;
	    case 'n':
		// create a hash with a non-null-terminated key, write to av[2]
		key = "nonterm";
		h = hash_new(HASH_MEMHASH);
		hash_insert(h, key, 7, "value", 6);
		hash_toFile(h, file);
		break;
	    default: goto usage;
	}
	if (h) hash_free(h);
	return (0);
}
