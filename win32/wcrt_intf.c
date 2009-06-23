/* %K% Copyright (c) 1999 Andrew Chang */
/* This file interface with the win32 C run time library */
/*
 * In general, we view the application running in a layered environment
 * (see diagram below).
 * In the BitMover's Application Library Layer, we
 * use only "BitMover's path name format": i.e All path name have forward slash
 * BitMover's path name is like Unix path, but "drive-letter colon"
 * is also allowed. (e.g. C:/temp)
 * In the Win32 C runtime layer, All path name have backward slash
 * We sometime call the backward slash format the NT path format.
 *
 * For the code in User Interface and Application Library layers,
 * all calls to the C runtime is remapped (with #define) to
 * a glue function in the Win32 transition layers
 *
 * We use nt2bmname and bm2ntname to translate the path
 * when a function is called across layer boundary
 *
 * | User interface Layer
 * | (This layer allows both NT & Unix name)
 * | Things like admin.c ci.c co.c sits in this layers
 * --------------------------------------------------
 * | BitMover Appication library Layer
 * | (All Path name is in BitMover format)
 * | Things in slib sits in this layers
 * --------------------------------------------------
 * | Win32 transition layers
 * | In this layer, we always return path name to upper layers
 * | in BitMover format. We always transltas pathname to NT format
 * | when we call down to the lower layer)
 * | Things in wcrt_intf.c & wapi_intf.c sits in this layers
 * ---------------------------------------------------
 * | Win32 C runtime layers
 * | (All path name has backward slash)
 * | Microsoft VC++ C run time stuff sits in this laters
 * | ------------------------------
 * | Win32 basic API layer
 * | Thing like "MapViewOfFile()" sits in this layer
 * ----------------------------------------------------
 *
 *
 * Note:  Two invariants are enforced for functions in the
 *	  Win32 transition layer:
 *	  a)	Path name is always translated to NT format before
 *		calling down to Win32 runtime layer,
 *	  b)	Path name is always translated to BitMover format
 *		before returning back to upper layers.
 *
 *				Andrew Chang awc@bitmover.com 1998
 */

#define	_RE_MAP_H_	/* Don't remap API */
#include "system.h"

/*
 * Scan ofn, replace all ochar to nchar, result is in nfn
 * caller is responsible to ensure nfn is at least as big as ofn.
 */
char *
_switch_char(const char *ofn, char *nfn, char ochar, char nchar)
{
	const	char *p;
	char	*q;

	if (!ofn) return (0);
	
	/*
	 * Simply replace all ochar with nchar
	 */
	if (ofn == nfn) {
		/* inplace edit, don't write if we don't have to */
		for (q = nfn; *q; q++) if (*q == ochar) *q = nchar;
	} else {
		p = ofn;
		q = nfn;
		while (1) {
			*q++ = (*p == ochar) ? nchar : *p;
			if (!*p) break;
			++p;
		}
	}
	return (nfn);
}


/*
 * Our version of open, does two additional things
 * a) Turn off inherite flag
 * b) Translate Bitmover filename to Win32 (i.e NT) path name
 */
int
nt_open(const char *filename, int flag, int pmode)
{
	int	fd, save;

	flag |= _O_NOINHERIT;
	fd = _open(filename, flag, pmode);
	if (fd < 0) {
		save = errno;
		if ((GetLastError() == ERROR_ACCESS_DENIED) &&
		    (flag & O_CREAT) && (flag & O_EXCL) &&
		    (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES)) {
			errno = EEXIST;
		} else {
			errno = save;	/* use errno set by _open */
		}
	}
	return (fd);
}


int
nt_dup(int fd)
{
	int new_fd;

	new_fd = (dup)(fd);
	if (new_fd >= 0) make_fd_uninheritable(new_fd);
	return (new_fd);
}

int
nt_dup2(int fd1, int fd2)
{
	int rc;

	rc = (dup2)(fd1, fd2);
	if (rc == -1) return (-1);
	make_fd_uninheritable(fd2);
	return (fd2);
}

int
nt_access(const char *file, int mode)
{
	DWORD	attrs = GetFileAttributes(file);
	char	*buf;

	if (attrs == INVALID_FILE_ATTRIBUTES) {
		(void)GetLastError(); /* set errno */
		if ((mode & X_OK) == X_OK) {
			buf = aprintf("%s.exe", file);
			attrs = GetFileAttributes(buf);
			free(buf);
			if (attrs != INVALID_FILE_ATTRIBUTES) {
				/* No need to test for W_OK.
				 * X_OK is never used with W_OK (I hope)
				 */
				return (0);
			}
		}
		return (-1);
	}
	if ((attrs & FILE_ATTRIBUTE_READONLY) && (mode & W_OK)) {
		errno = EACCES;
		return (-1);
	}
	return (0);
}

int
nt_mkdir (char *dirname)
{
	char	*p;
	int	rc;

	if (rc = _mkdir(dirname)) return (rc);

	/* make SCCS dirs hidden */
	if (p = strrchr(dirname, '/')) {
		++p;
	} else {
		p = dirname;
	}
	if (strcasecmp(p, "SCCS") == 0) {
		SetFileAttributes(dirname, FILE_ATTRIBUTE_HIDDEN);
	}
	return (0);
}

/*
 * Create a name for a tmp file
 */
char *
nt_tmpnam(void)
{
	static 	char tmpfbuf[1024];

	sprintf(tmpfbuf, "%s%s", nt_tmpdir(), tmpnam(0));

	/*
	 * Because tmpnam(0) return path in NT format
	 */
	nt2bmfname(tmpfbuf, tmpfbuf);
	return (tmpfbuf);
}

int
mkstemp(char *template)
{
	int	fd;
	char	*result;

	result = _mktemp(template);
	if (!result) return (-1);
	fd = _open(result, _O_CREAT|_O_EXCL|_O_BINARY|_O_SHORT_LIVED, 0600);
	if (fd >= 0) {
		nt2bmfname(result, result);
	} else {
		perror(result);
	}
	return (fd);
}

/*
 * Return ture if the path name is a full path name
 */
int
nt_is_full_path_name(char *path)
{
	/*
	 * Account for NT's
	 * a) \\remote_host\directory\filename format
	 * b) drive:\filename format
	 */
	if ((path[0] == '/') || (path[0] == '\\') || (path[1] == ':')) {
		return (1);	/* Yup. it's a full path name */
	}
	return (0);		/* Nope, not a full path name  */
}

int
pipe(int fd[2], int pipe_size)
{
	if (pipe_size == 0) pipe_size = 512;
	return (_pipe(fd, pipe_size, _O_BINARY|_O_NOINHERIT));
}

#undef	GetLastError

DWORD
bk_GetLastError(void)
{
	DWORD	ret = GetLastError();
	char	*p;
	static	int debug = ~0;

	errno = err2errno(ret);
	if (debug == ~0) {
		p = getenv("BK_DEBUG_LAST_ERROR");
		debug = (p && *p) ? 1 : 0;
	}

	if (debug) fprintf(stderr, "GetLastError() = %u\n", ret);
	return (ret);
}
