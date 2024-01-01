cmake_minimum_required(VERSION 3.10)
project(cat-feeder LANGUAGES C)

set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE feeder-sources
    src/*.c
    include/*.h
)

add_executable(feeder main.c)

add_library(feeder-lib ${feeder-sources})

# find_package(Threads REQUIRED QUIET)

target_include_directories(feeder-lib
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/feeder>
        $<INSTALL_INTERFACE:include/feeder>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# target_link_libraries(bldc-control
#     PRIVATE
#         Threads::Threads)

# get_target_property(node-computing_includes node-computing INCLUDE_DIRECTORIES)

# if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#     add_subdirectory(tests)
# endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(feeder
    PRIVATE
        feeder-lib)