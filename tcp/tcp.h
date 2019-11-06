#ifndef _TCL_TCL_H
#define _TCL_TCL_H

int	tcp_server(char *addr, int port, int quiet);
int	tcp_connect(char *host, int port);
int	tcp_accept(int sock);
void	tcp_ndelay(int sock, int val);
void	tcp_reuse(int sock);
void	tcp_keepalive(int sock);
int	udp_server(char *addr, int port, int quiet);
int	udp_connect(char *host, int port);
int	readable(int fd, int sec);
int	sockport(int s);
char	*sockaddr(int);
int	isLocalHost(char *h);
char	*hostaddr(char *);
int	tcp_pair(int fds[2]);
char	*peeraddr(int s);
int	issock(int);

#endif
