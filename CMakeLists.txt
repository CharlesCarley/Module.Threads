cmake_minimum_required(VERSION 3.15)
project(Threads)

# Use C++-17 by default.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the module path to include scripts
# CMake - Contains project local modules.
#
# CMake/Modules - Contains project independent modules.
#   GitUpdate.cmake invokes python ${Threads_SOURCE_DIR}/gitupdate.py
#   to pull all submodules and update to head.
#   If this fails, it will attempt to invoke git directly to pull 
#   submodules. 
list(APPEND CMAKE_MODULE_PATH 
  ${Threads_SOURCE_DIR}/CMake 
  ${Threads_SOURCE_DIR}/CMake/Modules
)
include(${Threads_SOURCE_DIR}/CMake/Configure.cmake)
if (NOT Configure_SUCCEEDED)
   return()
endif()



if (Threads_ExternalTarget)
    set(TargetFolders ${Threads_TargetFolders})
    set(TargetName    ${Threads_TargetName})
    set(TargetGroup   ${Threads_TargetGroup})
else()
    add_subdirectory(CMake)
    add_subdirectory(Internal/Utils)

    set(TargetFolders TRUE)
    set(TargetName Threads)
    set(TargetGroup )

    set(Threads_INCLUDE  ${Threads_SOURCE_DIR})
    set(Threads_LIBRARY  ${TargetName})
endif()

add_subdirectory(Threads)

if (Threads_BUILD_TEST)
    if (NOT Threads_ExternalTarget)
        add_gtest_source(Extern/ThirdParty)
    endif()

    set(TargetGroup Units)
    add_subdirectory(Test)
endif()
