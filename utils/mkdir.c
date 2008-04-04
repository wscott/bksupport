#include "system.h"

/*
 * Given a pathname, make the directory.
 */
int
mkdirp(char *dir)
{
	char	*t;
	int	ret;

	if (mkdir(dir, 0777) == 0) return (0);
	for (t = dir; *t; t++) {
		if ((*t != '/') || (t == dir)) continue;
		*t = 0;
		if (ret = mkdir(dir, 0777)) {
			*t = '/';
			return (ret);
		}
		*t = '/';
	}
	return (mkdir(dir, 0777));
}

/*
 * Given a pathname:
 * return 0 if access(2) indicates mkdirp() will succeed
 * return -1 if access(2) indicates mkdirp() will fail, and set errno.
 */
int
test_mkdirp(char *dir)
{
	char	buf[MAXPATH];
	char	*t;
	int	ret;

	if (IsFullPath(dir)) {
		strcpy(buf, dir);
	} else {
		strcpy(buf, "./");
		strcat(buf, dir);
	}
	while ((ret = access(buf, W_OK)) == -1 && (errno == ENOENT)) {
		t = strrchr(buf, '/');
		unless (t) break;
		*t = 0;
	}
	return (ret);
}

/*
 * given a pathname, create the dirname if it doesn't exist.
 */
int
mkdirf(char *file)
{
	char	*s;
	int	ret;

	unless (s = strrchr(file, '/')) return (0);
	*s = 0;
	if (isdir_follow(file)) {
		*s = '/';
		return (0);
	}
	ret = mkdirp(file);
	*s = '/';
	return (ret);
}
