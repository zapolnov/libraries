
#define _GNU_SOURCE 1
#define _NETBSD_SOURCE 1
#define __BSD_VISIBLE 1
#define _BSD_TYPES 1
#define _DARWIN_C_SOURCE 1

#ifdef _WIN32
#define MS_WINDOWS
#define MS_WIN32 1
#define NT_THREADS
#define HAVE_USABLE_WCHAR_T
typedef int pid_t;
#endif

#ifdef _WIN64
#define MS_WIN64
#endif

#ifdef _WIN32
#ifdef MS_WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#define HAVE_SSIZE_T 1
#endif

#define PREFIX "@PREFIX@"
#define HAVE_LONG_LONG 1
#define HAVE_PROTOTYPES 1
#define HAVE_STDARG_PROTOTYPES 1
#define HAVE_INT32_T 1
#define HAVE_INT64_T 1
#define HAVE_UINT32_T 1
#define HAVE_UINT64_T 1
#cmakedefine WITH_THREAD 1
#define WITH_PYMALLOC 1
#define Py_USING_UNICODE 1
#define Py_UNICODE_SIZE 2
#define PY_UNICODE_TYPE uint16_t
#define PY_FORMAT_LONG_LONG "ll"

#cmakedefine HAVE_ERRNO_H 1
#cmakedefine HAVE_NCURSES_H 1
#cmakedefine HAVE_TERM_H 1
#cmakedefine HAVE_SYS_STAT_H 1
#cmakedefine HAVE_SYS_TIME_H 1
#cmakedefine HAVE_SYS_UTIME_H 1
#cmakedefine HAVE_SYS_FILE_H 1
#cmakedefine HAVE_SYS_SELECT_H 1
#cmakedefine HAVE_SYS_PARAM_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_SYS_UTSNAME_H 1
#cmakedefine HAVE_SYS_STATVFS_H 1
#cmakedefine HAVE_SYS_TERMIO_H 1
#cmakedefine HAVE_SYS_POLL_H 1
#cmakedefine HAVE_SYS_EPOLL_H 1
#cmakedefine HAVE_SYS_EVENT_H 1
#cmakedefine HAVE_SYS_WAIT_H 1
#cmakedefine HAVE_SYS_RESOURCE_H 1
#cmakedefine HAVE_SYS_LOADAVG_H 1
#cmakedefine HAVE_SYSEXITS_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_FCNTL_H 1
#cmakedefine HAVE_GRP_H 1
#cmakedefine HAVE_DIRENT_H 1
#cmakedefine HAVE_DIRECT_H 1
#cmakedefine HAVE_SIGNAL_H 1
#cmakedefine HAVE_LANGINFO_H 1
#cmakedefine HAVE_IEEEFP_H 1
#cmakedefine HAVE_WCHAR_H 1
#cmakedefine HAVE_UNISTD_H 1
#cmakedefine HAVE_STDDEF_H 1
#cmakedefine HAVE_PTHREAD_H 1
#cmakedefine HAVE_UTIME_H 1
#cmakedefine HAVE_LIBINTL_H 1
#cmakedefine HAVE_IO_H 1
#cmakedefine HAVE_PROCESS_H 1
#cmakedefine HAVE_PTY_H 1
#cmakedefine HAVE_LIBUTIL_H 1
#cmakedefine HAVE_UTIL_H 1

#cmakedefine SIZEOF_INT @SIZEOF_INT@
#cmakedefine SIZEOF_LONG @SIZEOF_LONG@
#cmakedefine SIZEOF_LONG_LONG @SIZEOF_LONG_LONG@
#cmakedefine SIZEOF_FLOAT @SIZEOF_FLOAT@
#cmakedefine SIZEOF_DOUBLE @SIZEOF_DOUBLE@
#cmakedefine SIZEOF_VOID_P @SIZEOF_VOID_P@
#cmakedefine SIZEOF_SIZE_T @SIZEOF_SIZE_T@
#cmakedefine SIZEOF_OFF_T @SIZEOF_OFF_T@
#cmakedefine SIZEOF_WCHAR_T @SIZEOF_WCHAR_T@
#cmakedefine SIZEOF_TIME_T @SIZEOF_TIME_T@
#cmakedefine SIZEOF_FPOS_T @SIZEOF_FPOS_T@
#cmakedefine SIZEOF_HKEY @SIZEOF_HKEY@

#cmakedefine HAVE_FINITE 1
#cmakedefine HAVE_GETCWD 1
#cmakedefine HAVE_GETWD 1
#cmakedefine HAVE_HYPOT 1
#cmakedefine HAVE_COPYSIGN 1
#cmakedefine HAVE_ROUND 1
#cmakedefine HAVE_REALPATH 1
#cmakedefine HAVE_READLINK 1
#cmakedefine HAVE_GETTIMEOFDAY 1
#cmakedefine HAVE_SYSCONF 1
#cmakedefine HAVE_GETADDRINFO 1
#cmakedefine HAVE_GETNAMEINFO 1
#cmakedefine HAVE_GETHOSTBYNAME_R 1
#cmakedefine HAVE_STATVFS 1
#cmakedefine HAVE_FSTATVFS 1
#cmakedefine HAVE_FSEEKO 1
#cmakedefine HAVE_FSEEK64 1
#cmakedefine HAVE_FTELLO 1
#cmakedefine HAVE_FTELL64 1
#cmakedefine HAVE_GETPAGESIZE 1
#cmakedefine HAVE_KQUEUE 1
#cmakedefine HAVE_POLL 1
#cmakedefine HAVE_EPOLL 1
#cmakedefine HAVE_CTERMID 1
#cmakedefine HAVE_CTERMID_R 1
#cmakedefine HAVE_FSYNC 1
#cmakedefine HAVE_SOCKETPAIR 1
#cmakedefine HAVE_FCHDIR 1
#cmakedefine HAVE_SYMLINK 1
#cmakedefine HAVE_LINK 1
#cmakedefine HAVE_CHROOT 1
#cmakedefine HAVE_WAIT3 1
#cmakedefine HAVE_WAIT4 1
#cmakedefine HAVE_WAITPID 1
#cmakedefine HAVE_SELECT 1
#cmakedefine HAVE_FORK 1
#cmakedefine HAVE_DLOPEN 1
#cmakedefine HAVE_TMPNAM 1
#cmakedefine HAVE_TMPNAM_R 1
#cmakedefine HAVE_TEMPNAM 1
#cmakedefine HAVE_TMPFILE 1
#cmakedefine HAVE_MMAP 1
#cmakedefine HAVE_MREMAP 1
#cmakedefine HAVE_SETVBUF 1
#cmakedefine HAVE_UNAME 1
#cmakedefine HAVE_WCSCOLL 1
#cmakedefine HAVE_LSTAT 1
#cmakedefine HAVE_FTIME 1
#cmakedefine HAVE_PUTENV 1
#cmakedefine HAVE_STRFTIME 1
#cmakedefine HAVE_MKTIME 1
#cmakedefine HAVE_NICE 1
#cmakedefine HAVE_PAUSE 1
/*#cmakedefine HAVE_DUP2 1*/
#cmakedefine HAVE_OPENPTY 1
#cmakedefine HAVE_FORKPTY 1
#cmakedefine HAVE_GETPEERNAME 1

#cmakedefine HAVE_UINTPTR_T 1
#cmakedefine HAVE_SSIZE_T 1
#cmakedefine TIME_WITH_SYS_TIME 1
#cmakedefine HAVE_SNPRINTF 1
#cmakedefine HAVE_ADDRINFO 1
#cmakedefine HAVE_SOCKADDR_STORAGE 1
#cmakedefine HAVE_STAT_TV_NSEC 1
#cmakedefine HAVE_STAT_TV_NSEC2 1
#cmakedefine HAVE_SOCKADDR_SA_LEN 1
#cmakedefine HAVE_ALTZONE 1
#cmakedefine HAVE_HSTRERROR 1
#cmakedefine HAVE_FDATASYNC 1

#cmakedefine VA_LIST_IS_ARRAY 1
#cmakedefine HAVE_GCC_ASM_FOR_X87 1
#cmakedefine HAVE_GETC_UNLOCKED 1
#cmakedefine HAVE_GETHOSTBYNAME_R_6_ARG 1
#cmakedefine HAVE_GETHOSTBYNAME_R_5_ARG 1
#cmakedefine HAVE_GETHOSTBYNAME_R_3_ARG 1
#cmakedefine HAVE_INET_ATON 1
#cmakedefine HAVE_INET_PTON 1
