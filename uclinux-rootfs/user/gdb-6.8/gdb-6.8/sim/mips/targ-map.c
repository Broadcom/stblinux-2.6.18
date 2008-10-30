/* Target value mapping utilities needed by the simulator and gdb.  */
/* This file is machine generated by gentmap.c.  */

#include <errno.h>
#include <fcntl.h>
#include "ansidecl.h"
#include "gdb/callback.h"
#include "targ-vals.h"

/* syscall mapping table */
CB_TARGET_DEFS_MAP cb_init_syscall_map[] = {
  { -1, -1 }
};

/* errno mapping table */
CB_TARGET_DEFS_MAP cb_init_errno_map[] = {
#ifdef E2BIG
  { E2BIG, TARGET_E2BIG },
#endif
#ifdef EACCES
  { EACCES, TARGET_EACCES },
#endif
#ifdef EADDRINUSE
  { EADDRINUSE, TARGET_EADDRINUSE },
#endif
#ifdef EADDRNOTAVAIL
  { EADDRNOTAVAIL, TARGET_EADDRNOTAVAIL },
#endif
#ifdef EADV
  { EADV, TARGET_EADV },
#endif
#ifdef EAFNOSUPPORT
  { EAFNOSUPPORT, TARGET_EAFNOSUPPORT },
#endif
#ifdef EAGAIN
  { EAGAIN, TARGET_EAGAIN },
#endif
#ifdef EALREADY
  { EALREADY, TARGET_EALREADY },
#endif
#ifdef EBADE
  { EBADE, TARGET_EBADE },
#endif
#ifdef EBADF
  { EBADF, TARGET_EBADF },
#endif
#ifdef EBADFD
  { EBADFD, TARGET_EBADFD },
#endif
#ifdef EBADMSG
  { EBADMSG, TARGET_EBADMSG },
#endif
#ifdef EBADR
  { EBADR, TARGET_EBADR },
#endif
#ifdef EBADRQC
  { EBADRQC, TARGET_EBADRQC },
#endif
#ifdef EBADSLT
  { EBADSLT, TARGET_EBADSLT },
#endif
#ifdef EBFONT
  { EBFONT, TARGET_EBFONT },
#endif
#ifdef EBUSY
  { EBUSY, TARGET_EBUSY },
#endif
#ifdef ECHILD
  { ECHILD, TARGET_ECHILD },
#endif
#ifdef ECHRNG
  { ECHRNG, TARGET_ECHRNG },
#endif
#ifdef ECOMM
  { ECOMM, TARGET_ECOMM },
#endif
#ifdef ECONNABORTED
  { ECONNABORTED, TARGET_ECONNABORTED },
#endif
#ifdef ECONNREFUSED
  { ECONNREFUSED, TARGET_ECONNREFUSED },
#endif
#ifdef ECONNRESET
  { ECONNRESET, TARGET_ECONNRESET },
#endif
#ifdef EDEADLK
  { EDEADLK, TARGET_EDEADLK },
#endif
#ifdef EDEADLOCK
  { EDEADLOCK, TARGET_EDEADLOCK },
#endif
#ifdef EDESTADDRREQ
  { EDESTADDRREQ, TARGET_EDESTADDRREQ },
#endif
#ifdef EDOM
  { EDOM, TARGET_EDOM },
#endif
#ifdef EDOTDOT
  { EDOTDOT, TARGET_EDOTDOT },
#endif
#ifdef EDQUOT
  { EDQUOT, TARGET_EDQUOT },
#endif
#ifdef EEXIST
  { EEXIST, TARGET_EEXIST },
#endif
#ifdef EFAULT
  { EFAULT, TARGET_EFAULT },
#endif
#ifdef EFBIG
  { EFBIG, TARGET_EFBIG },
#endif
#ifdef EHOSTDOWN
  { EHOSTDOWN, TARGET_EHOSTDOWN },
#endif
#ifdef EHOSTUNREACH
  { EHOSTUNREACH, TARGET_EHOSTUNREACH },
#endif
#ifdef EIDRM
  { EIDRM, TARGET_EIDRM },
#endif
#ifdef EINPROGRESS
  { EINPROGRESS, TARGET_EINPROGRESS },
#endif
#ifdef EINTR
  { EINTR, TARGET_EINTR },
#endif
#ifdef EINVAL
  { EINVAL, TARGET_EINVAL },
#endif
#ifdef EIO
  { EIO, TARGET_EIO },
#endif
#ifdef EISCONN
  { EISCONN, TARGET_EISCONN },
#endif
#ifdef EISDIR
  { EISDIR, TARGET_EISDIR },
#endif
#ifdef EL2HLT
  { EL2HLT, TARGET_EL2HLT },
#endif
#ifdef EL2NSYNC
  { EL2NSYNC, TARGET_EL2NSYNC },
#endif
#ifdef EL3HLT
  { EL3HLT, TARGET_EL3HLT },
#endif
#ifdef EL3RST
  { EL3RST, TARGET_EL3RST },
#endif
#ifdef ELBIN
  { ELBIN, TARGET_ELBIN },
#endif
#ifdef ELIBACC
  { ELIBACC, TARGET_ELIBACC },
#endif
#ifdef ELIBBAD
  { ELIBBAD, TARGET_ELIBBAD },
#endif
#ifdef ELIBEXEC
  { ELIBEXEC, TARGET_ELIBEXEC },
#endif
#ifdef ELIBMAX
  { ELIBMAX, TARGET_ELIBMAX },
#endif
#ifdef ELIBSCN
  { ELIBSCN, TARGET_ELIBSCN },
#endif
#ifdef ELNRNG
  { ELNRNG, TARGET_ELNRNG },
#endif
#ifdef ELOOP
  { ELOOP, TARGET_ELOOP },
#endif
#ifdef EMFILE
  { EMFILE, TARGET_EMFILE },
#endif
#ifdef EMLINK
  { EMLINK, TARGET_EMLINK },
#endif
#ifdef EMSGSIZE
  { EMSGSIZE, TARGET_EMSGSIZE },
#endif
#ifdef EMULTIHOP
  { EMULTIHOP, TARGET_EMULTIHOP },
#endif
#ifdef ENAMETOOLONG
  { ENAMETOOLONG, TARGET_ENAMETOOLONG },
#endif
#ifdef ENETDOWN
  { ENETDOWN, TARGET_ENETDOWN },
#endif
#ifdef ENETRESET
  { ENETRESET, TARGET_ENETRESET },
#endif
#ifdef ENETUNREACH
  { ENETUNREACH, TARGET_ENETUNREACH },
#endif
#ifdef ENFILE
  { ENFILE, TARGET_ENFILE },
#endif
#ifdef ENMFILE
  { ENMFILE, TARGET_ENMFILE },
#endif
#ifdef ENOANO
  { ENOANO, TARGET_ENOANO },
#endif
#ifdef ENOBUFS
  { ENOBUFS, TARGET_ENOBUFS },
#endif
#ifdef ENOCSI
  { ENOCSI, TARGET_ENOCSI },
#endif
#ifdef ENODATA
  { ENODATA, TARGET_ENODATA },
#endif
#ifdef ENODEV
  { ENODEV, TARGET_ENODEV },
#endif
#ifdef ENOENT
  { ENOENT, TARGET_ENOENT },
#endif
#ifdef ENOEXEC
  { ENOEXEC, TARGET_ENOEXEC },
#endif
#ifdef ENOLCK
  { ENOLCK, TARGET_ENOLCK },
#endif
#ifdef ENOLINK
  { ENOLINK, TARGET_ENOLINK },
#endif
#ifdef ENOMEM
  { ENOMEM, TARGET_ENOMEM },
#endif
#ifdef ENOMSG
  { ENOMSG, TARGET_ENOMSG },
#endif
#ifdef ENONET
  { ENONET, TARGET_ENONET },
#endif
#ifdef ENOPKG
  { ENOPKG, TARGET_ENOPKG },
#endif
#ifdef ENOPROTOOPT
  { ENOPROTOOPT, TARGET_ENOPROTOOPT },
#endif
#ifdef ENOSPC
  { ENOSPC, TARGET_ENOSPC },
#endif
#ifdef ENOSR
  { ENOSR, TARGET_ENOSR },
#endif
#ifdef ENOSTR
  { ENOSTR, TARGET_ENOSTR },
#endif
#ifdef ENOSYS
  { ENOSYS, TARGET_ENOSYS },
#endif
#ifdef ENOTBLK
  { ENOTBLK, TARGET_ENOTBLK },
#endif
#ifdef ENOTCONN
  { ENOTCONN, TARGET_ENOTCONN },
#endif
#ifdef ENOTDIR
  { ENOTDIR, TARGET_ENOTDIR },
#endif
#ifdef ENOTEMPTY
  { ENOTEMPTY, TARGET_ENOTEMPTY },
#endif
#ifdef ENOTSOCK
  { ENOTSOCK, TARGET_ENOTSOCK },
#endif
#ifdef ENOTSUP
  { ENOTSUP, TARGET_ENOTSUP },
#endif
#ifdef ENOTTY
  { ENOTTY, TARGET_ENOTTY },
#endif
#ifdef ENOTUNIQ
  { ENOTUNIQ, TARGET_ENOTUNIQ },
#endif
#ifdef ENXIO
  { ENXIO, TARGET_ENXIO },
#endif
#ifdef EOPNOTSUPP
  { EOPNOTSUPP, TARGET_EOPNOTSUPP },
#endif
#ifdef EPERM
  { EPERM, TARGET_EPERM },
#endif
#ifdef EPFNOSUPPORT
  { EPFNOSUPPORT, TARGET_EPFNOSUPPORT },
#endif
#ifdef EPIPE
  { EPIPE, TARGET_EPIPE },
#endif
#ifdef EPROCLIM
  { EPROCLIM, TARGET_EPROCLIM },
#endif
#ifdef EPROTO
  { EPROTO, TARGET_EPROTO },
#endif
#ifdef EPROTONOSUPPORT
  { EPROTONOSUPPORT, TARGET_EPROTONOSUPPORT },
#endif
#ifdef EPROTOTYPE
  { EPROTOTYPE, TARGET_EPROTOTYPE },
#endif
#ifdef ERANGE
  { ERANGE, TARGET_ERANGE },
#endif
#ifdef EREMCHG
  { EREMCHG, TARGET_EREMCHG },
#endif
#ifdef EREMOTE
  { EREMOTE, TARGET_EREMOTE },
#endif
#ifdef EROFS
  { EROFS, TARGET_EROFS },
#endif
#ifdef ESHUTDOWN
  { ESHUTDOWN, TARGET_ESHUTDOWN },
#endif
#ifdef ESOCKTNOSUPPORT
  { ESOCKTNOSUPPORT, TARGET_ESOCKTNOSUPPORT },
#endif
#ifdef ESPIPE
  { ESPIPE, TARGET_ESPIPE },
#endif
#ifdef ESRCH
  { ESRCH, TARGET_ESRCH },
#endif
#ifdef ESRMNT
  { ESRMNT, TARGET_ESRMNT },
#endif
#ifdef ESTALE
  { ESTALE, TARGET_ESTALE },
#endif
#ifdef ETIME
  { ETIME, TARGET_ETIME },
#endif
#ifdef ETIMEDOUT
  { ETIMEDOUT, TARGET_ETIMEDOUT },
#endif
#ifdef ETOOMANYREFS
  { ETOOMANYREFS, TARGET_ETOOMANYREFS },
#endif
#ifdef ETXTBSY
  { ETXTBSY, TARGET_ETXTBSY },
#endif
#ifdef EUNATCH
  { EUNATCH, TARGET_EUNATCH },
#endif
#ifdef EUSERS
  { EUSERS, TARGET_EUSERS },
#endif
#ifdef EWOULDBLOCK
  { EWOULDBLOCK, TARGET_EWOULDBLOCK },
#endif
#ifdef EXDEV
  { EXDEV, TARGET_EXDEV },
#endif
#ifdef EXFULL
  { EXFULL, TARGET_EXFULL },
#endif
  { 0, 0 }
};

/* open flags mapping table */
CB_TARGET_DEFS_MAP cb_init_open_map[] = {
#ifdef O_ACCMODE
  { O_ACCMODE, TARGET_O_ACCMODE },
#endif
#ifdef O_APPEND
  { O_APPEND, TARGET_O_APPEND },
#endif
#ifdef O_CREAT
  { O_CREAT, TARGET_O_CREAT },
#endif
#ifdef O_EXCL
  { O_EXCL, TARGET_O_EXCL },
#endif
#ifdef O_NOCTTY
  { O_NOCTTY, TARGET_O_NOCTTY },
#endif
#ifdef O_NONBLOCK
  { O_NONBLOCK, TARGET_O_NONBLOCK },
#endif
#ifdef O_RDONLY
  { O_RDONLY, TARGET_O_RDONLY },
#endif
#ifdef O_RDWR
  { O_RDWR, TARGET_O_RDWR },
#endif
#ifdef O_SYNC
  { O_SYNC, TARGET_O_SYNC },
#endif
#ifdef O_TRUNC
  { O_TRUNC, TARGET_O_TRUNC },
#endif
#ifdef O_WRONLY
  { O_WRONLY, TARGET_O_WRONLY },
#endif
  { -1, -1 }
};
