cmake_minimum_required(VERSION 3.10)
project(cat-feeder LANGUAGES C)

set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(feeder main.c)

include(cmake/modules/feeder.cmake)

# get_target_property(node-computing_includes node-computing INCLUDE_DIRECTORIES)

# if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#     add_subdirectory(tests)
# endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(feeder
    PRIVATE
        feeder-lib)

configure_file(config.json .)