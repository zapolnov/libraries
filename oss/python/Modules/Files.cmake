
set(base_modules_files
    Modules/addrinfo.h
    Modules/_codecsmodule.c
    Modules/_functoolsmodule.c
    Modules/_localemodule.c
    Modules/_math.c
    Modules/_math.h
    Modules/_sre.c
    Modules/_weakref.c
    Modules/errnomodule.c
    Modules/main.c
    Modules/gcmodule.c
    Modules/getaddrinfo.c
    Modules/getbuildinfo.c
    Modules/getnameinfo.c
    Modules/getpath.c
    Modules/operator.c
    Modules/posixmodule.c
    Modules/posixmodule.h
    Modules/rotatingtree.c
    Modules/rotatingtree.h
    Modules/signalmodule.c
    Modules/sre.h
    Modules/sre_constants.h
    Modules/unicodedata_db.h
    Modules/unicodename_db.h
    Modules/zipimport.c
    Modules/zlibmodule.c
)

set_source_files_properties(Modules/getaddrinfo.c PROPERTIES HEADER_FILE_ONLY YES)
set_source_files_properties(Modules/getnameinfo.c PROPERTIES HEADER_FILE_ONLY YES)
source_group("Source Files\\Modules" FILES ${base_modules_files})
