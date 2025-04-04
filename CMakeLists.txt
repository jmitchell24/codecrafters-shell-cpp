cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)
set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

file (
        GLOB_RECURSE SOURCE_FILES
        src/*.cpp
        src/*.hpp
        src/*.h
        src/*.c

)


add_executable(shell ${SOURCE_FILES}
        src/scanner.hpp
        src/scanner.cpp
        src/command.hpp
        src/command.cpp
        src/shell.hpp
        src/shell.cpp
        src/autocomplete.hpp
        src/autocomplete.cpp
        src/escapes.hpp
        src/path.hpp
        src/path.cpp)

include(FetchContent)

#
# ut
#
# url       : https://gitlab.com/james-mitchell-dev/ut.git
# mirror 1  : http://jm-server.local:10880/jm-admin-server/ut.git
#
FetchContent_Declare(
        ut
        GIT_REPOSITORY https://gitlab.com/james-mitchell-dev/ut.git)
FetchContent_MakeAvailable(ut)
target_link_libraries(shell PRIVATE ut)


