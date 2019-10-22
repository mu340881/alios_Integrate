	
#ifndef _SYS_ERRNO_H_
#define _SYS_ERRNO_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;
#define __errno_r(ptr) ((ptr)->_errno)



//extern int *__errno(void);
//#define errno (*__errno())

/*
 * POSIX Error codes
 */

#define EPERM 1		/* Not owner */
#define ENOENT 2	/* No such file or directory */
#define ESRCH 3		/* No such context */
#define EINTR 4		/* Interrupted system call */
#define EIO 5		/* I/O error */
#define ENXIO 6		/* No such device or address */
#define E2BIG 7		/* Arg list too long */
#define ENOEXEC 8       /* Exec format error */
#define EBADF 9		/* Bad file number */
#define ECHILD 10       /* No children */
#define EAGAIN 11       /* No more contexts */
#define ENOMEM 12       /* Not enough core */
#define EACCES 13       /* Permission denied */
#define EFAULT 14       /* Bad address */
#define ENOTEMPTY 15    /* Directory not empty */
#define EBUSY 16	/* Mount device busy */
#define EEXIST 17       /* File exists */
#define EXDEV 18	/* Cross-device link */
#define ENODEV 19       /* No such device */
#define ENOTDIR 20      /* Not a directory */
#define EISDIR 21       /* Is a directory */
#define EINVAL 22       /* Invalid argument */
#define ENFILE 23       /* File table overflow */
#define EMFILE 24       /* Too many open files */
#define ENOTTY 25       /* Not a typewriter */
#define ENAMETOOLONG 26 /* File name too long */
#define EFBIG 27	/* File too large */
#define ENOSPC 28       /* No space left on device */
#define ESPIPE 29       /* Illegal seek */
#define EROFS 30	/* Read-only file system */
#define EMLINK 31       /* Too many links */
#define EPIPE 32	/* Broken pipe */
#define EDEADLK 33      /* Resource deadlock avoided */
#define ENOLCK 34       /* No locks available */
#define ENOTSUP 35      /* Unsupported value */
#define EMSGSIZE 36     /* Message size */

/* ANSI math software */
#define EDOM 37   /* Argument too large */
#define ERANGE 38 /* Result too large */

/* ipc/network software */

/* argument errors */
#define EDESTADDRREQ 40    /* Destination address required */
#define EPROTOTYPE 41      /* Protocol wrong type for socket */
#define ENOPROTOOPT 42     /* Protocol not available */
#define EPROTONOSUPPORT 43 /* Protocol not supported */
#define ESOCKTNOSUPPORT 44 /* Socket type not supported */
#define EOPNOTSUPP 45      /* Operation not supported on socket */
#define EPFNOSUPPORT 46    /* Protocol family not supported */
#define EAFNOSUPPORT 47    /* Addr family not supported */
#define EADDRINUSE 48      /* Address already in use */
#define EADDRNOTAVAIL 49   /* Can't assign requested address */
#define ENOTSOCK 50	/* Socket operation on non-socket */

/* operational errors */
#define ENETUNREACH 51  /* Network is unreachable */
#define ENETRESET 52    /* Network dropped connection on reset */
#define ECONNABORTED 53 /* Software caused connection abort */
#define ECONNRESET 54   /* Connection reset by peer */
#define ENOBUFS 55      /* No buffer space available */
#define EISCONN 56      /* Socket is already connected */
#define ENOTCONN 57     /* Socket is not connected */
#define ESHUTDOWN 58    /* Can't send after socket shutdown */
#define ETOOMANYREFS 59 /* Too many references: can't splice */
#define ETIMEDOUT 60    /* Connection timed out */
#define ECONNREFUSED 61 /* Connection refused */
#define ENETDOWN 62     /* Network is down */
#define ETXTBSY 63      /* Text file busy */
#define ELOOP 64	/* Too many levels of symbolic links */
#define EHOSTUNREACH 65 /* No route to host */
#define ENOTBLK 66      /* Block device required */
#define EHOSTDOWN 67    /* Host is down */

/* non-blocking and interrupt i/o */
#define EINPROGRESS 68 /* Operation now in progress */
#define EALREADY 69    /* Operation already in progress */
#define EWOULDBLOCK EAGAIN /* Operation would block */

#define ENOSYS 71 /* Function not implemented */

/* aio errors (should be under posix) */
#define ECANCELED 72 /* Operation canceled */

#define ERRMAX 81

/* specific STREAMS errno values */

#define ENOSR 74   /* Insufficient memory */
#define ENOSTR 75  /* STREAMS device required */
#define EPROTO 76  /* Generic STREAMS error */
#define EBADMSG 77 /* Invalid STREAMS message */
#define ENODATA 78 /* Missing expected message data */
#define ETIME 79   /* STREAMS timeout occurred */
#define ENOMSG 80  /* Unexpected message type */



#define ENSROK    0 /* DNS server returned answer with no data */
#define ENSRNODATA  160 /* DNS server returned answer with no data */
#define ENSRFORMERR 161 /* DNS server claims query was misformatted */
#define ENSRSERVFAIL 162  /* DNS server returned general failure */
#define ENSRNOTFOUND 163  /* Domain name not found */
#define ENSRNOTIMP  164 /* DNS server does not implement requested operation */
#define ENSRREFUSED 165 /* DNS server refused query */
#define ENSRBADQUERY 166  /* Misformatted DNS query */
#define ENSRBADNAME 167 /* Misformatted domain name */
#define ENSRBADFAMILY 168 /* Unsupported address family */
#define ENSRBADRESP 169 /* Misformatted DNS reply */
#define ENSRCONNREFUSED 170 /* Could not contact DNS servers */
#define ENSRTIMEOUT 171 /* Timeout while contacting DNS servers */
#define ENSROF    172 /* End of file */
#define ENSRFILE  173 /* Error reading file */
#define ENSRNOMEM 174 /* Out of memory */
#define ENSRDESTRUCTION 175 /* Application terminated lookup */
#define ENSRQUERYDOMAINTOOLONG  176 /* Domain name is too long */
#define ENSRCNAMELOOP 177 /* Domain name is too long */
#define ENOTSUPP      178 /*no support */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_ERRNO_H_ */

