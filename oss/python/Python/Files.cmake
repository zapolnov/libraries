
set(python_files
    Python/Python-ast.c
    Python/_warnings.c
    Python/asdl.c
    Python/ast.c
    Python/atof.c
    Python/bltinmodule.c
    Python/ceval.c
    Python/codecs.c
    Python/compile.c
    Python/dtoa.c
    Python/dup2.c
    Python/errors.c
    Python/formatter_string.c
    Python/formatter_unicode.c
    Python/frozen.c
    Python/future.c
    Python/getargs.c
    Python/getcompiler.c
    Python/getcopyright.c
    Python/getcwd.c
    Python/getopt.c
    Python/getplatform.c
    Python/getversion.c
    Python/graminit.c
    Python/import.c
    Python/importdl.h
    Python/mactoolboxglue.c
    Python/marshal.c
    Python/modsupport.c
    Python/mysnprintf.c
    Python/mystrtoul.c
    Python/peephole.c
    Python/pyarena.c
    Python/pyctype.c
    Python/pyfpe.c
    Python/pymath.c
    Python/pystate.c
    Python/pystrcmp.c
    Python/pystrtod.c
    Python/pythonrun.c
    Python/random.c
    Python/strdup.c
    Python/strtod.c
    Python/structmember.c
    Python/symtable.c
    Python/sysmodule.c
    Python/thread.c
    Python/thread_atheos.h
    Python/thread_beos.h
    Python/thread_cthread.h
    Python/thread_foobar.h
    Python/thread_lwp.h
    Python/thread_nt.h
    Python/thread_os2.h
    Python/thread_pth.h
    Python/thread_pthread.h
    Python/thread_sgi.h
    Python/thread_solaris.h
    Python/thread_wince.h
    Python/traceback.c
)

if(NOT APPLE)
    set_source_files_properties(Python/mactoolboxglue.c PROPERTIES HEADER_FILE_ONLY YES)
endif()

if(WIN32 OR HAVE_DUP2)
    set_source_files_properties(Python/dup2.c PROPERTIES HEADER_FILE_ONLY YES)
endif()

if(WIN32 OR HAVE_GETCWD)
    set_source_files_properties(Python/getcwd.c PROPERTIES HEADER_FILE_ONLY YES)
endif()
