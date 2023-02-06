# -----------------------------------------------------------------------------
#   Copyright (c) Charles Carley.
#
#   This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
# ------------------------------------------------------------------------------
set(TargetName Threads)

include(GroupSet)

set_group(
    Threads_SRC
    Source:*.cpp
    Header:*.h
)


if (WIN32)
    set_group(
        Threads_SRC_PLAT
        Source/Windows:Windows/*.cpp
        Header/Windows:Windows/*.h
    )
    set_ro_group(
        Threads_EX
        Source/Posix:Posix/*.cpp
        Header/Posix:Posix/*.h
        Header/Posix:Windows/stubs/*.h
    )

    set(CrossCOMPILE FALSE)
    if (CrossCOMPILE)
        include_directories(Windows/stubs)
    endif()

else()

	find_package(Threads REQUIRED)
    
    set_group(
        Threads_SRC_PLAT
        Source/Posix:Posix/*.cpp
        Header/Posix:Posix/*.h
    )
    set(Threads_PLAT_LIBS pthread)
endif()

include_directories(${Threads_SOURCE_DIR} ${Utils_INCLUDE})

add_library(
    ${TargetName} 
    ${Threads_SRC}
    ${Threads_SRC_PLAT}
    ${Threads_EX}
)

target_link_libraries(${TargetName} ${Utils_LIBRARY} ${Threads_PLAT_LIBS})
set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetGroup}")
