/* %W% Copyright (c) 1999 Zack Weinberg */
#ifndef _BK_UNIX_H
#define _BK_UNIX_H
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pwd.h>
#include <dirent.h>
#include <netdb.h>
#include <unistd.h>
#include <utime.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <resolv.h>
#ifdef _AIX
#include <sys/select.h>
#endif

/*
 * Local config starts here.
 */
#define	PATH_DELIM	':'
#define EDITOR		"vi"
#define	DEV_NULL	"/dev/null" /* for write */
#define	NULL_FILE	"/dev/null" /* for read */
#define DEV_TTY		"/dev/tty"
#define	ROOT_USER	"root"
#define	TMP_PATH	"/tmp"
#define	IsFullPath(f)	((f)[0] == '/')
#define	patheq(a, b)	streq(a, b)
#define	strieq(a, b)	!strcasecmp(a, b)
#define pathneq(a, b, n) strneq(a, b, n)
#define sameuser(a,b)	(!strcmp(a, b))
#define	fileBusy(f)	(0) 
#define wishConsoleVisible()	(1)
#define	mixedCasePath()	1
#define reserved(f)	(0)		/* No reserved names on unix */

#define GetShortPathName(p, q, s) 	/* Unlike win32, 
					 * Unix has no short path name
					 */
#define	fullname	full_name	/* Darwin libc collision */

/* These functions are a "no-op" on unix */
#define localName2bkName(x, y)
#define	make_fd_uninheritable(fd)  fcntl(fd, F_SETFD, 1)
#define	mkpipe(p, size)	pipe(p)
#define	setmode(a, b)

#define	unlink(f)	smartUnlink(f)
#define	rename(o, n)	smartRename(o, n)
#define	realmkdir(d, m)	(mkdir)(d, m)
#define	mkdir(d, m)	smartMkdir(d, m)
#define	closesocket(i)	close(i)

#define	win32()		0
#define	isWin2000()	0
#define	win_supported()	1


#ifdef	__APPLE__
#define	macosx()	1
#else
#define	macosx()	0
#endif

char	*fast_getcwd(char *buf, int len);

#endif
