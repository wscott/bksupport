#include "system.h"

/*
 * Given a pathname, make the directory.
 */
int
mkdirp(char *dir)
{
	char	*t = 0;
	int	more;

	while (1) {
		unless (mkdir(dir, 0777)) break;
		if (errno != ENOENT) return (-1);

		/*
		 * some component in the pathname doesn't exist
		 * go back one at a time until we find it
		 * for any other errno, we can just quit
		 */
		if (t) {
			*t-- = '/';
			while ((t > dir) && (*t != '/')) --t;
			if (t == dir) return (-1);
		} else {
			unless ((t = strrchr(dir, '/')) && (t > dir)) {
				return (-1);
			}
		}
		*t = 0;
	}
	/*
	 * Now if we had to go back to make a one of the component
	 * directories, we walk forward and build the path in order
	 */
	while (t) {
		*t++ = '/';
		while (*t && (*t != '/')) t++;
		if (more = *t) *t = 0;
		if (mkdir(dir, 0777)) return (-1);
		unless (more) t = 0;
	}
	return (0);
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
	while (((ret = access(buf, W_OK)) == -1) && (errno == ENOENT)) {
		t = strrchr(buf, '/');
		unless (t) break;
		*t = 0;
	}
	if (!ret && !writable(buf)) ret = -1;
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
