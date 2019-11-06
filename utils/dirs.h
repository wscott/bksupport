char	**getdir(char *dir);
typedef	int	filefn(char *file, char type, void *data);
typedef	int	dirfn(char *file, void *data);

typedef struct {
	filefn	*file;	/* called on each inode in a dir (file, link, dir) */
	dirfn	*dir;	/* called after all inodes in a dir are done */
	dirfn	*tail;	/* called after all inodes in the subtree are done */
} walkfns;
int walkdir(char *dir, walkfns fn, void *token);

