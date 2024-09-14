# $Id: CMakeLists.txt 667318 2023-05-12 15:49:10Z lavr $

set(CONNEXT_SRC_C
    ncbi_crypt ncbi_dblb ncbi_localnet)
if (UNIX)
    set(CONNEXT_SRC_C ${CONNEXT_SRC_C} ncbi_ifconf)
endif(UNIX)

NCBI_add_library(connext xconnext)
NCBI_add_subdirectory(test)
