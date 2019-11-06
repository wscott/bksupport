#ifndef WIN32
#define	_P_WAIT		0 	/* for spawnvp() */
#define	_P_NOWAIT 	1	/* for spawnvp() */
#define	_P_DETACH 	2	/* for spawnvp() */
#define	P_WAIT		_P_WAIT
#define	P_NOWAIT 	_P_NOWAIT
#define	P_DETACH 	_P_DETACH
#endif

#define	spawnvp bk_spawnvp

extern void	(*spawn_preHook)(int flags, char *av[]);
pid_t	bk_spawnvp(int flags, char *cmdname, char *av[]);
pid_t	spawnvp_ex(int flags, char *cmdname, char *av[]);
pid_t	spawnvpio(int *fd0, int *fd1, int *fd2, char *av[]);
int	spawn_filterPipeline(char **cmds);

