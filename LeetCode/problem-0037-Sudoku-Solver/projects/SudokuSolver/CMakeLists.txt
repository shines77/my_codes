CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(SudokuSolver)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message("----------------------------------")

message("-------------- Env ---------------")
message("  CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("  CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("  PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("  PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message("----------------------------------")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
endif()

if (NOT MSVC)
    ## -Wall -Werror -Wextra -Wno-format -Wno-unused-function
    set(CMAKE_CXX_FLAGS_DEFAULT "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unused-function -Wno-deprecated-declarations -Wno-unused-variable -fPIC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEFAULT} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEFAULT} -g -pg -D_DEBUG")
endif()

if (WIN32)
    add_compile_options("-D_WIN32_WINNT=0x0601")
    add_compile_options("-D_CRT_SECURE_NO_WARNINGS")
    set(EXTRA_LIBS ${EXTRA_LIBS} ws2_32 mswsock)
endif()

if (UNIX)
    set(EXTRA_LIBS ${EXTRA_LIBS} pthread)
else()
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()

include_directories(include)
include_directories(src)

set(SOURCE_FILES
    src/ice_sudoku_solver.c
    src/SudokuSolver_v1.cpp
    src/Solution.cpp
    )

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)

add_executable(SudokuSolver ${SOURCE_FILES})
target_link_libraries(SudokuSolver ${EXTRA_LIBS})
