#############################################################################
# $Id: amd64-windows-msvc-192.cmake 666172 2023-04-21 17:26:17Z gouriano $
#############################################################################
#############################################################################
##
##  NCBI CMake wrapper
##  MSVC 192 (VS2019) build settings

include_guard(GLOBAL)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_SYSTEM_PROCESSOR AMD64)
set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")

include(${CMAKE_CURRENT_LIST_DIR}/amd64-windows-msvc.cmake)
