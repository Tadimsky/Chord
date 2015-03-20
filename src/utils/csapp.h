/* $begin csapp.h */
#ifndef __CSAPP_H__
#define __CSAPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>


/* Default file permissions are DEF_MODE & ~DEF_UMASK */
/* $begin createmasks */
#define DEF_MODE   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
#define DEF_UMASK  S_IWGRP|S_IWOTH
/* $end createmasks */

/* Simplifies calls to bind(), connect(), and accept() */
/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */

/* Persistent state for the robust I/O (Rio) package */
/* $begin rio_t */
#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;                /* descriptor for this internal buf */
    int rio_cnt;               /* unread bytes in internal buf */
    char *rio_bufptr;          /* next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
} rio_t;
/* $end rio_t */

/* External variables */
extern int h_errno;    /* defined by BIND for DNS errors */ 
extern char **environ; /* defined by libc */

/* Misc constants */
#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

/* Our own error-handling functions */
void unix_error(char *msg);
void posix_error(int code, char *msg);
void dns_error(char *msg);

void log_unix_error(char *msg);
void log_dns_error(char *msg);

/* Signal wrappers */
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);
void Sigdelset(sigset_t *set, int signum);
int Sigismember(const sigset_t *set, int signum);

/* Unix I/O wrappers */
int Open(const char *pathname, int flags, mode_t mode);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
off_t Lseek(int fildes, off_t offset, int whence);
void Close(int fd);
int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, 
	   struct timeval *timeout);
int Dup2(int fd1, int fd2);
void Stat(const char *filename, struct stat *buf);
void Fstat(int fd, struct stat *buf) ;

/* Dynamic storage allocation wrappers */
void *Malloc(size_t size);
void *Realloc(void *ptr, size_t size);
void *Calloc(size_t nmemb, size_t size);
void Free(void *ptr);

/* Sockets interface wrappers */
int Socket(int domain, int type, int protocol);
int Setsockopt(int s, int level, int optname, const void *optval, int optlen);
int Bind(int sockfd, struct sockaddr *my_addr, int addrlen);
int Listen(int s, int backlog);
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, struct sockaddr *serv_addr, int addrlen);

/* DNS wrappers */
struct hostent *Gethostbyname(const char *name);
struct hostent *Gethostbyaddr(const char *addr, int len, int type);

/* Pthreads thread control wrappers */
void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp);
void Pthread_join(pthread_t tid, void **thread_return);
void Pthread_cancel(pthread_t tid);
void Pthread_detach(pthread_t tid);
void Pthread_exit(void *retval);
pthread_t Pthread_self(void);
void Pthread_once(pthread_once_t *once_control, void (*init_function)());

/* Mutex wrappers */
void Pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
void Pthread_mutex_lock(pthread_mutex_t *mutex);
void Pthread_mutex_unlock(pthread_mutex_t *mutex);

/* Condition variable wrappers */
void Pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
void Pthread_cond_signal(pthread_cond_t *cond);
void Pthread_cond_broadcast(pthread_cond_t *cond);
void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int Pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
			   struct timespec *abstime);

/* Rio (Robust I/O) package */

class RIOHelper {
public:
	static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
	static ssize_t rio_readp(int fd, void *ptr, size_t nbytes);
	static ssize_t rio_writep(int fd, void *ptr, size_t nbytes);
	static ssize_t rio_readn(int fd, void *usrbuf, size_t n);
	static ssize_t rio_writen(int fd, void *usrbuf, size_t n);
	static void rio_readinitb(rio_t *rp, int fd);
	static ssize_t	rio_readnb(rio_t *rp, void *usrbuf, size_t n);
	static ssize_t	rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
};

class RIO {
public:
	/* Wrappers for Rio package */
	static ssize_t readp(int fd, void *ptr, size_t nbytes);
	static size_t writep(int fd, void *usrbuf, size_t n);
	static size_t writeString(int fd, const std::string* message);
	static ssize_t readn(int fd, void *usrbuf, size_t n);
	static size_t writen(int fd, void *usrbuf, size_t n);
	static void readinitb(rio_t *rp, int fd);
	static ssize_t readnb(rio_t *rp, void *usrbuf, size_t n);
	static ssize_t readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
	static ssize_t skiplineb(rio_t *rp);
};

class RIOBuffered {
private:
	rio_t myRioData;
public:
	RIOBuffered(int fd);
	~RIOBuffered();

	std::string readLine();
	std::string readBytes(size_t amount);
	size_t writeLine(const std::string* msg);
	int getFD();
};



/* Client/server helper functions */
int open_clientfd(char *hostname, int portno);
int open_listenfd(int portno);

/* Wrappers for client/server helper functions */
int Open_clientfd(const char *hostname, int port);
int Open_listenfd(int port); 

void ignore();

#endif /* __CSAPP_H__ */
/* $end csapp.h */
