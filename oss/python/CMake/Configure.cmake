#
# Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com).
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

set(PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")

option(Z_PYTHON_WITH_THREADS "Build Python with multithreading support" OFF)
if(Z_PYTHON_WITH_THREADS)
    set(WITH_THREAD YES)
endif()

add_definitions(-D_GNU_SOURCE=1)
add_definitions(-D_NETBSD_SOURCE=1)
add_definitions(-D__BSD_VISIBLE=1)
add_definitions(-D_BSD_TYPES=1)
add_definitions(-D_DARWIN_C_SOURCE=1)

list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE=1)
list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_NETBSD_SOURCE=1)
list(APPEND CMAKE_REQUIRED_DEFINITIONS -D__BSD_VISIBLE=1)
list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_BSD_TYPES=1)
list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_DARWIN_C_SOURCE=1)

list(APPEND CMAKE_REQUIRED_INCLUDES "stdio.h")
list(APPEND CMAKE_REQUIRED_INCLUDES "time.h")

if(WIN32)
    include_directories(PC)
    add_definitions(-DNT_THREADS)
    add_definitions(-DMS_WINDOWS)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DWIN32_LEAN_AND_MEAN)
    list(APPEND CMAKE_REQUIRED_INCLUDES "windows.h")
endif()

find_library(LIBM m)
include(FindThreads)

if(LIBM)
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${LIBM})
endif()

include(CheckCCompilerFlag)
if(MSVC)
    check_c_compiler_flag("/w" HAVE_DISABLE_ALL_WARNINGS)
    if(HAVE_DISABLE_ALL_WARNINGS)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
    endif()
else()
    check_c_compiler_flag("-w" HAVE_DISABLE_ALL_WARNINGS)
    if(HAVE_DISABLE_ALL_WARNINGS)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    endif()
endif()

include(CheckIncludeFile)
check_include_file("errno.h" HAVE_ERRNO_H)
check_include_file("ncurses.h" HAVE_NCURSES_H)
check_include_file("term.h" HAVE_TERM_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("sys/utime.h" HAVE_SYS_UTIME_H)
check_include_file("sys/file.h" HAVE_SYS_FILE_H)
check_include_file("sys/select.h" HAVE_SYS_SELECT_H)
check_include_file("sys/param.h" HAVE_SYS_PARAM_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/utsname.h" HAVE_SYS_UTSNAME_H)
check_include_file("sys/statvfs.h" HAVE_SYS_STATVFS_H)
check_include_file("sys/termio.h" HAVE_SYS_TERMIO_H)
check_include_file("sys/poll.h" HAVE_SYS_POLL_H)
check_include_file("sys/epoll.h" HAVE_SYS_EPOLL_H)
check_include_file("sys/event.h" HAVE_SYS_EVENT_H)
check_include_file("sys/wait.h" HAVE_SYS_WAIT_H)
check_include_file("sys/resource.h" HAVE_SYS_RESOURCE_H)
check_include_file("sys/loadavg.h" HAVE_SYS_LOADAVG_H)
check_include_file("sysexits.h" HAVE_SYSEXITS_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("grp.h" HAVE_GRP_H)
check_include_file("dirent.h" HAVE_DIRENT_H)
check_include_file("direct.h" HAVE_DIRECT_H)
check_include_file("signal.h" HAVE_SIGNAL_H)
check_include_file("langinfo.h" HAVE_LANGINFO_H)
check_include_file("ieeefp.h" HAVE_IEEEFP_H)
check_include_file("wchar.h" HAVE_WCHAR_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("stddef.h" HAVE_STDDEF_H)
check_include_file("pthread.h" HAVE_PTHREAD_H)
check_include_file("utime.h" HAVE_UTIME_H)
check_include_file("libintl.h" HAVE_LIBINTL_H)
check_include_file("io.h" HAVE_IO_H)
check_include_file("process.h" HAVE_PROCESS_H)
check_include_file("pty.h" HAVE_PTY_H)
check_include_file("libutil.h" HAVE_LIBUTIL_H)
check_include_file("util.h" HAVE_UTIL_H)

if(HAVE_STDINT_H)
    list(APPEND CMAKE_REQUIRED_INCLUDES "stdint.h")
endif()

if(HAVE_INTTYPES_H)
    list(APPEND CMAKE_REQUIRED_INCLUDES "inttypes.h")
endif()

if(HAVE_SYS_TYPES_H)
    list(APPEND CMAKE_REQUIRED_INCLUDES "sys/types.h")
endif()

include(CheckTypeSize)
check_type_size("int" SIZEOF_INT)
check_type_size("long" SIZEOF_LONG)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size("float" SIZEOF_FLOAT)
check_type_size("double" SIZEOF_DOUBLE)
check_type_size("void*" SIZEOF_VOID_P)
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("off_t" SIZEOF_OFF_T)
check_type_size("wchar_t" SIZEOF_WCHAR_T)
check_type_size("time_t" SIZEOF_TIME_T)

if(WIN32)
    set(SIZEOF_FPOS_T 8)
    check_type_size("HKEY" SIZEOF_HKEY)
else()
    check_type_size("fpos_t" SIZEOF_FPOS_T)
endif()

include(CheckFunctionExists)
check_function_exists("finite" HAVE_FINITE)
check_function_exists("getcwd" HAVE_GETCWD)
check_function_exists("getwd" HAVE_GETWD)
check_function_exists("hypot" HAVE_HYPOT)
check_function_exists("copysign" HAVE_COPYSIGN)
check_function_exists("round" HAVE_ROUND)
check_function_exists("realpath" HAVE_REALPATH)
check_function_exists("readlink" HAVE_READLINK)
check_function_exists("gettimeofday" HAVE_GETTIMEOFDAY)
check_function_exists("sysconf" HAVE_SYSCONF)
check_function_exists("getaddrinfo" HAVE_GETADDRINFO)
check_function_exists("getnameinfo" HAVE_GETNAMEINFO)
check_function_exists("gethostbyname_r" HAVE_GETHOSTBYNAME_R)
check_function_exists("statvfs" HAVE_STATVFS)
check_function_exists("fstatvfs" HAVE_FSTATVFS)
check_function_exists("fseeko" HAVE_FSEEKO)
check_function_exists("fseek64" HAVE_FSEEK64)
check_function_exists("ftello" HAVE_FTELLO)
check_function_exists("ftell64" HAVE_FTELL64)
check_function_exists("getpagesize" HAVE_GETPAGESIZE)
check_function_exists("kqueue" HAVE_KQUEUE)
check_function_exists("poll" HAVE_POLL)
check_function_exists("ctermid" HAVE_CTERMID)
check_function_exists("ctermid_r" HAVE_CTERMID_R)
check_function_exists("fsync" HAVE_FSYNC)
check_function_exists("socketpair" HAVE_SOCKETPAIR)
check_function_exists("fchdir" HAVE_FCHDIR)
check_function_exists("symlink" HAVE_SYMLINK)
check_function_exists("link" HAVE_LINK)
check_function_exists("chroot" HAVE_CHROOT)
check_function_exists("wait3" HAVE_WAIT3)
check_function_exists("wait4" HAVE_WAIT4)
check_function_exists("waitpid" HAVE_WAITPID)
check_function_exists("select" HAVE_SELECT)
check_function_exists("fork" HAVE_FORK)
check_function_exists("dlopen" HAVE_DLOPEN)
if(WIN32)
    check_function_exists("tmpnam" HAVE_TMPNAM)
    check_function_exists("tmpnam_r" HAVE_TMPNAM_R)
    check_function_exists("tempnam" HAVE_TEMPNAM)
endif()
check_function_exists("tmpfile" HAVE_TMPFILE)
check_function_exists("mmap" HAVE_MMAP)
check_function_exists("mremap" HAVE_MREMAP)
check_function_exists("setvbuf" HAVE_SETVBUF)
check_function_exists("uname" HAVE_UNAME)
check_function_exists("wcscoll" HAVE_WCSCOLL)
check_function_exists("lstat" HAVE_LSTAT)
check_function_exists("ftime" HAVE_FTIME)
check_function_exists("putenv" HAVE_PUTENV)
check_function_exists("strftime" HAVE_STRFTIME)
check_function_exists("mktime" HAVE_MKTIME)
check_function_exists("nice" HAVE_NICE)
check_function_exists("pause" HAVE_PAUSE)
check_function_exists("dup2" HAVE_DUP2)
check_function_exists("openpty" HAVE_OPENPTY)
check_function_exists("forkpty" HAVE_FORKPTY)

if(NOT WIN32)
    check_function_exists("getpeername" HAVE_GETPEERNAME)
else()
    set(HAVE_GETPEERNAME YES)
endif()

set(includes)
foreach(include ${CMAKE_REQUIRED_INCLUDES})
    set(includes "${includes}#include <${include}>\n")
endforeach()

include(CheckCSourceCompiles)
check_c_source_compiles("${includes}#include <stddef.h>\nint main() { uintptr_t v = 0; return 0; }\n" HAVE_UINTPTR_T)
check_c_source_compiles("${includes}#include <stddef.h>\nint main() { ssize_t v = 0; return 0; }\n" HAVE_SSIZE_T)
check_c_source_compiles("#include <sys/time.h>\n#include <time.h>\nint main() { return 0; }\n" TIME_WITH_SYS_TIME)
check_c_source_compiles("#include <stdio.h>\nint main() { char p[10]; return snprintf(p, 10, \"%d\", 0); }\n" HAVE_SNPRINTF)
check_c_source_compiles("#include <sys/epoll.h>\nint main() { struct epoll_event e; return epoll_wait(0, &e, 0, 0); }\n" HAVE_EPOLL)
check_c_source_compiles("#include <netdb.h>\nint main() { struct addrinfo a; return 0; }\n" HAVE_ADDRINFO)
check_c_source_compiles("#include <sys/types.h>\n#include <sys/socket.h>\nint main() { struct sockaddr_storage s; return 0; }\n" HAVE_SOCKADDR_STORAGE)
check_c_source_compiles("#include <sys/stat.h>\nint main() { struct stat st; st.st_mtim.tv_nsec = 1; return 0; }\n" HAVE_STAT_TV_NSEC)
check_c_source_compiles("#include <sys/stat.h>\nint main() { struct stat st; st.st_mtimespec.tv_nsec = 1; return 0; }\n" HAVE_STAT_TV_NSEC2)
check_c_source_compiles("#include <sys/types.h>\n#include <sys/socket.h>\nint main() { struct sockaddr x; x.sa_len = 0; return 0; }\n" HAVE_SOCKADDR_SA_LEN)
check_c_source_compiles("#include <time.h>\nint main() { return altzone; }\n" HAVE_ALTZONE)
check_c_source_compiles("#include <netdb.h>\nint main() { void* p = hstrerror; hstrerror(0); return 0; }\n" HAVE_HSTRERROR)
check_c_source_compiles("#include <unistd.h>\nint main() { void*p = fdatasync; fdatasync(0); return 0; }\n" HAVE_FDATASYNC)

check_c_source_compiles("
    #include <stdarg.h>
    int main() {
        va_list list1, list2; list1 = list2;
        return 0;
    }
" VA_LIST_IS_POINTER)

if(NOT VA_LIST_IS_POINTER)
    set(VA_LIST_IS_ARRAY YES)
endif()

check_c_source_compiles("
    int main() {
        unsigned short cw;
        __asm__ __volatile__ (\"fnstcw %0\" : \"=m\" (cw));
        __asm__ __volatile__ (\"fldcw %0\" : : \"m\" (cw));
        return 0;
    }
" HAVE_GCC_ASM_FOR_X87)

check_c_source_compiles("
    #include <stdio.h>
    int main() {
        FILE *f = fopen(\"/dev/null\", \"r\");
        flockfile(f);
        getc_unlocked(f);
        funlockfile(f);
        return 0;
    }
" HAVE_GETC_UNLOCKED)

check_c_source_compiles("
    #include <netdb.h>
    int main() {
        char *name;
        struct hostent *he, *res;
        char buffer[2048];
        int buflen = 2048;
        int h_errnop;
        (void)gethostbyname_r(name, he, buffer, buflen, &res, &h_errnop);
        return 0;
    }
" HAVE_GETHOSTBYNAME_R_6_ARG)

check_c_source_compiles("
    #include <netdb.h>
    int main() {
        char *name;
        struct hostent *he;
        char buffer[2048];
        int buflen = 2048;
        int h_errnop;
        (void)gethostbyname_r(name, he, buffer, buflen, &h_errnop);
        return 0;
    }
" HAVE_GETHOSTBYNAME_R_5_ARG)

check_c_source_compiles("
    #include <netdb.h>
    int main() {
        char *name;
        struct hostent *he;
        struct hostent_data data;
        (void)gethostbyname_r(name, he, &data);
        return 0;
    }
" HAVE_GETHOSTBYNAME_R_3_ARG)

check_c_source_compiles("
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    int main() {
        void* p = inet_aton;
        inet_aton(0,0);
        return 0;
    }
" HAVE_INET_ATON)

check_c_source_compiles("
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    int main() {
        void* p = inet_pton;
        return 0;
    }
" HAVE_INET_PTON)

configure_file(pyconfig.h.cmake "${CMAKE_CURRENT_BINARY_DIR}/pyconfig.h")
