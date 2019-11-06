extern	int bk_trace;
int	indent(void);
void	trace_init(char *prog);
void	trace_msg(char *file, int line,
    const char *function, u32 bits, char *fmt, ...)
#ifdef __GNUC__
     __attribute__((format (__printf__, 5, 6)))
#endif
	;
int	trace_this(char *file, int line, const char *function, u32 bits);
void	trace_free(void);

/*
 * the env var gets split and turned into these.  We check this selector
 * first, it's much faster than a bunch of glob matches.
 *
 * T_TMP traces shouldn't be checked in, they are there as a way for you
 * to pepper the code with those and find your problem; then clean them up.
 */
#define	TR_DEFAULT	0x00000001	// "default", on by default
#define	TR_FS_LAYER	0x00000002	// "fs", on if selected
#define	TR_PERF		0x00000004	// "perf", on if selected
#define	TR_IF		0x00000008	// "if", on if selected
#define	TR_DEBUG	0x00000010	// "debug", on if selected
#define	TR_LOCK		0x00000020	// "lock", on if selected
#define	TR_NESTED	0x00000040	// "nested", on if selected
#define	TR_TMP		0x00000080	// "tmp", on if selected
#define	TR_O1		0x00000100	// "o1" (paging), on if selected
#define	TR_PROJ		0x00000200	// "proj", on if selected
#define	TR_SHIP		0x00000400	// "ship", ships to customers
#define	TR_CMD		0x00000800	// log command start/stop
#define	TR_SCCS		0x00001000	// most of the sccs_* interfaces
#define	TR_INIT		0x00002000	// show sccs_init's
/* Hey!  You!  Adding a flag!  Make it match in trace_init() */
#define	TR_ALL		0xffffffff	// "all", easy way to select everything

#ifdef	USE_TRACE
#define	TRACE(args...)	\
	do { if (unlikely(bk_trace))					    \
	    trace_msg(__FILE__, __LINE__, __FUNCTION__, TR_DEFAULT, ##args);\
	} while (0)
#define	T_NUM(args...)	\
	do { if (unlikely(bk_trace))				     \
		trace_msg(__FILE__, __LINE__, __FUNCTION__, ##args); \
	} while (0)
#define	IF_TRACE							\
	if (unlikely(bk_trace) && trace_this(__FILE__, __LINE__, __FUNCTION__, TR_IF))
#else
#define	TRACE(args...) {}
#define	T_NUM(args...) {}
#define	IF_TRACE	if (0)
#endif

#define	T_FS(args...)		T_NUM(TR_FS_LAYER, ##args)
#define	T_PERF(args...)		T_NUM(TR_PERF, ##args)
#define	T_DEBUG(args...)	T_NUM(TR_DEBUG, ##args)
#define	T_LOCK(args...)		T_NUM(TR_LOCK, ##args)
#define	T_NESTED(args...)	T_NUM(TR_NESTED, ##args)
#define	T_TMP(args...)		T_NUM(TR_TMP, ##args)
#define	T_O1(args...)		T_NUM(TR_O1, ##args)
#define	T_PROJ(args...)		T_NUM(TR_PROJ, ##args)
#define	T_SCCS(args...)		T_NUM(TR_SCCS, ##args)
#define	T_INIT(args...)		T_NUM(TR_INIT, ##args)
#define	T_CMD(args...) \
	do { if (unlikely(bk_trace))					   \
		trace_msg(__FILE__,__LINE__, __FUNCTION__, TR_CMD, ##args);\
	} while (0)
#define	T_SHIP(args...) \
	do { if (unlikely(bk_trace))					    \
		trace_msg(__FILE__,__LINE__, __FUNCTION__, TR_SHIP, ##args);\
	} while (0)

