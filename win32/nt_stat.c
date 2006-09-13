#define	_RE_MAP_H_	/* Don't remap API */
#include "system.h"

private	time_t	filetime2timet(FILETIME *ft);
private	int	attribute2mode(int attribute);

int
nt_stat(const char *file, struct stat *sb)
{
	WIN32_FILE_ATTRIBUTE_DATA	info;

	assert(sb);
	assert(file);
	memset(sb, 0, sizeof(sb));
	unless (GetFileAttributesEx(file, GetFileExInfoStandard, &info)) {
		switch (GetLastError()) {
			case ERROR_ACCESS_DENIED: errno = EACCES; break;
			case ERROR_FILE_NOT_FOUND: errno = ENOENT; break;
			default: errno = EINVAL;
		}
		return (-1);
	}
	sb->st_atime = filetime2timet(&info.ftLastAccessTime);
	sb->st_ctime = filetime2timet(&info.ftCreationTime);
	sb->st_mtime = filetime2timet(&info.ftLastWriteTime);

	sb->st_mode = attribute2mode(info.dwFileAttributes);

	sb->st_nlink = 1;

	/* We don't support files larger than 4GB on NTFS */
	assert(info.nFileSizeHigh == 0);
	sb->st_size = info.nFileSizeLow;

	return (0);
}

private inline FILETIME
int2ft(__int64 i)
{
	FILETIME	ret;

	ret.dwHighDateTime = (DWORD)(i >> 32);
	ret.dwLowDateTime = (DWORD)i;
	return (ret);
}

private inline __int64
ft2int(FILETIME *ft)
{
	__int64	ret;

	ret = ((__int64)ft->dwHighDateTime << 32) + ft->dwLowDateTime;
	return (ret);
}

private time_t
filetime2timet(FILETIME *ft)
{
	__int64	ftime = ft2int(ft);

	ftime /= (1000*1000*10);  /* 100 nano -> seconds */
	ftime -= 11644473600LL;		/* 1601 -> 1970 */

	return ((time_t)ftime);
}

private void
timet2filetime(time_t *t, FILETIME *ft)
{
	__int64	ftime = *t;

	ftime += 11644473600LL;		/* 1970 -> 1601 */
	ftime *= (1000*1000*10);  /* seconds -> 100 nano */

	*ft = int2ft(ftime);
}

private int
attribute2mode(int attribute)
{
	int mode = 0;

	if (FILE_ATTRIBUTE_DIRECTORY & attribute) {
		mode |=  _S_IFDIR;
	} else {
		mode |=  _S_IFREG;
	}
	if (FILE_ATTRIBUTE_READONLY & attribute) {
		mode |= 0444;
	} else {
		mode |= 0666;
	}
	return (mode);
}

int
nt_utime(char *file, struct utimbuf *ut)
{
	HANDLE	fh;
	FILETIME	atime, mtime;
	struct	utimbuf	now;

	fh = CreateFile(file, FILE_WRITE_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_DELETE, 0,
			OPEN_EXISTING, 0, 0);
	if (fh == INVALID_HANDLE_VALUE) {
err:		switch (GetLastError()) {
			case ERROR_ACCESS_DENIED: errno = EACCES; break;
			case ERROR_FILE_NOT_FOUND: errno = ENOENT; break;
			default: errno = EINVAL;
		}
		return (-1);
	}
	unless (ut) {
		now.actime = now.modtime = time(0);
		ut = &now;
	}
	timet2filetime(&ut->actime, &atime);
	timet2filetime(&ut->modtime, &mtime);

	unless (SetFileTime(fh, 0, &atime, &mtime)) goto err;
	safeCloseHandle(fh);
	return (0);
}

