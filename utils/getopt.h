#define getopt	mygetopt
#define optind	myoptind
#define optarg	myoptarg
#define	opterr	myopterr
#define	optopt	myoptopt

extern	int	optind;
extern	int	opterr;
extern	int	optopt;
extern	char	*optarg;

typedef struct {
	char	*name;		/* name w args ex: "url:" */
	int	ret;		/* return value from getopt */
} longopt;

#define	GETOPT_ERR	256	/* bad option */
#define	GETOPT_NOARG	257	/* missing argument */
int	getopt(int ac, char **av, char *opts, longopt *lopts);
void	getoptReset(void);
void	getoptConsumed(int n);


