file(GLOB_RECURSE feeder-sources
    src/*.c
    include/*.h
)

add_library(feeder-lib
    ${feeder-sources}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON/cJSON.c
)

target_include_directories(feeder-lib
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/feeder>
        $<INSTALL_INTERFACE:include/feeder>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(feeder-lib
    PRIVATE
        ${CJSON_LIB}
)
