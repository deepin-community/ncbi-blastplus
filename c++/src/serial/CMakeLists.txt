# $Id: CMakeLists.txt 656007 2022-09-13 15:14:27Z gouriano $

NCBI_add_library(serial cserial)
NCBI_add_subdirectory(datatool test soap grpc_integration)


