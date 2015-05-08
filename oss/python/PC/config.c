/* Module configuration */

/* This file contains the table of built-in modules.
   See init_builtin() in import.c. */

#include "Python.h"
#include "python_additional_module_list.h"

extern void initarray(void);
#ifndef MS_WINI64
extern void initaudioop(void);
#endif
extern void initbinascii(void);
extern void initcmath(void);
extern void initerrno(void);
extern void initfuture_builtins(void);
extern void initgc(void);
#ifndef MS_WINI64
extern void initimageop(void);
#endif
extern void initmath(void);
extern void init_md5(void);
extern void initnt(void);
extern void initoperator(void);
extern void initsignal(void);
extern void init_sha(void);
extern void init_sha256(void);
extern void init_sha512(void);
extern void initstrop(void);
extern void inittime(void);
extern void initthread(void);
extern void initcStringIO(void);
extern void initcPickle(void);
#ifdef WIN32
extern void initmsvcrt(void);
extern void init_locale(void);
#endif
extern void init_codecs(void);
extern void init_weakref(void);
extern void init_hotshot(void);
extern void initxxsubtype(void);
extern void initzipimport(void);
extern void init_random(void);
extern void inititertools(void);
extern void init_collections(void);
extern void init_heapq(void);
extern void init_bisect(void);
extern void init_symtable(void);
extern void initmmap(void);
extern void init_csv(void);
extern void init_sre(void);
extern void initparser(void);
extern void init_winreg(void);
extern void init_struct(void);
extern void initdatetime(void);
extern void init_functools(void);
extern void init_json(void);
extern void initposix(void);
extern void initzlib(void);

extern void init_multibytecodec(void);
extern void init_codecs_cn(void);
extern void init_codecs_hk(void);
extern void init_codecs_iso2022(void);
extern void init_codecs_jp(void);
extern void init_codecs_kr(void);
extern void init_codecs_tw(void);
extern void init_subprocess(void);
extern void init_lsprof(void);
extern void init_ast(void);
extern void init_io(void);
extern void initunicodedata(void);
extern void initfcntl(void);
extern void init_socket(void);
extern void initselect(void);
extern void _PyWarnings_Init(void);

/* tools/freeze/makeconfig.py marker for additional "extern" */
/* -- ADDMODULE MARKER 1 -- */

extern void PyMarshal_Init(void);
extern void initimp(void);

struct _inittab _PyImport_Inittab[] = {

#ifdef PYTHON_MODULE_array
    {"array", initarray},
#endif
#if 0
    {"_ast", init_ast},
#ifdef MS_WINDOWS
#ifndef MS_WINI64
    {"audioop", initaudioop},
#endif
#endif
#endif
#ifdef PYTHON_MODULE_binascii
    {"binascii", initbinascii},
#endif
#ifdef PYTHON_MODULE_cmath
    {"cmath", initcmath},
#endif
    {"errno", initerrno},
#ifdef PYTHON_MODULE_future_builtins
    {"future_builtins", initfuture_builtins},
#endif
    {"gc", initgc},
#ifndef MS_WINI64
#ifdef PYTHON_MODULE_imageop
    {"imageop", initimageop},
#endif
#endif
#ifdef PYTHON_MODULE_math
    {"math", initmath},
#endif
#if 0
    {"_md5", init_md5},
#endif
#ifdef _WIN32
    {"nt", initnt}, /* Use the NT os functions, not posix */
#else
    {"posix", initposix},
#endif
    {"operator", initoperator},
    {"signal", initsignal},
#if 0
    {"_sha", init_sha},
    {"_sha256", init_sha256},
    {"_sha512", init_sha512},
#endif
#ifdef PYTHON_MODULE_strop
    {"strop", initstrop},
#endif
#ifdef PYTHON_MODULE_time
    {"time", inittime},
#endif
#ifdef WITH_THREAD
#ifdef PYTHON_MODULE_thread
    {"thread", initthread},
#endif
#endif
#ifdef PYTHON_MODULE_cStringIO
    {"cStringIO", initcStringIO},
#endif
#ifdef PYTHON_MODULE_cPickle
    {"cPickle", initcPickle},
#endif
#ifdef WIN32
#ifdef PYTHON_MODULE_msvcrt
    {"msvcrt", initmsvcrt},
#endif
    {"_locale", init_locale},
#endif
    /* XXX Should _subprocess go in a WIN32 block?  not WIN64? */
#ifdef _WIN32
#if 0
    {"_subprocess", init_subprocess},
#endif
#endif

    {"_codecs", init_codecs},
    {"_weakref", init_weakref},
#if 0
    {"_hotshot", init_hotshot},
    {"_random", init_random},
    {"_bisect", init_bisect},
    {"_heapq", init_heapq},
    {"_lsprof", init_lsprof},
#endif
#ifdef PYTHON_MODULE_itertools
    {"itertools", inititertools},
#endif
#if 0
    {"_collections", init_collections},
    {"_symtable", init_symtable},
#endif
#ifndef _WIN32
#ifdef PYTHON_MODULE_mmap
    {"mmap", initmmap},
#endif
#endif
#if 0
    {"_csv", init_csv},
#endif
    {"_sre", init_sre},
#ifdef PYTHON_MODULE_parser
    {"parser", initparser},
#endif
#ifdef _WIN32
#if 0
    {"_winreg", init_winreg},
#endif
#endif
#if 0
    {"_struct", init_struct},
#endif
#ifdef PYTHON_MODULE_datetime
    {"datetime", initdatetime},
#endif
    {"_functools", init_functools},
#if 0
    {"_json", init_json},
#endif

#ifdef PYTHON_MODULE_xxsubtype
    {"xxsubtype", initxxsubtype},
#endif
    {"zipimport", initzipimport},
    {"zlib", initzlib},

    /* CJK codecs */
#if 0
    {"_multibytecodec", init_multibytecodec},
    {"_codecs_cn", init_codecs_cn},
    {"_codecs_hk", init_codecs_hk},
    {"_codecs_iso2022", init_codecs_iso2022},
    {"_codecs_jp", init_codecs_jp},
    {"_codecs_kr", init_codecs_kr},
    {"_codecs_tw", init_codecs_tw},
#endif

/* tools/freeze/makeconfig.py marker for additional "_inittab" entries */
/* -- ADDMODULE MARKER 2 -- */

    /* This module "lives in" with marshal.c */
    {"marshal", PyMarshal_Init},

    /* This lives it with import.c */
    {"imp", initimp},

    /* These entries are here for sys.builtin_module_names */
    {"__main__", NULL},
    {"__builtin__", NULL},
    {"sys", NULL},
    {"exceptions", NULL},
    {"_warnings", _PyWarnings_Init},

#if 0
    {"_io", init_io},
#endif
#ifdef PYTHON_MODULE_unicodedata
    {"unicodedata", initunicodedata},
#endif
#ifndef _WIN32
#ifdef PYTHON_MODULE_fcntl
    {"fcntl", initfcntl},
#endif
#endif
#if 0
    {"_socket", init_socket},
#endif
#ifdef PYTHON_MODULE_select
    {"select", initselect},
#endif

    /* Sentinel */
    {0, 0}
};
