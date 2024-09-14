# $Id: CMakeLists.txt 658230 2022-10-27 09:35:46Z stakhovv $

NCBI_add_library(edit)
NCBI_add_subdirectory(unit_test)
NCBI_add_subdirectory(pubmed_fetch)
NCBI_add_subdirectory(pubmed_citmatch)
NCBI_add_subdirectory(test_remote_updater)
