cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wno-error")

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/force_rebuild"
        COMMAND rm -f "${CMAKE_SOURCE_DIR}/link/libenginend.so"
        COMMAND rm -rf "${CMAKE_SOURCE_DIR}/include/enginend"
)
add_library(enginend SHARED ${ENGINE_SOURCES} "${CMAKE_BINARY_DIR}/force_rebuild"
        ../include/json.h)
target_link_directories(
        enginend PUBLIC
        "${CMAKE_SOURCE_DIR}/link")
set(ENGINE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(ENGINE_EXPORT_DIR "${CMAKE_SOURCE_DIR}/include/enginend")
set_target_properties(enginend PROPERTIES
        CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/link"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/link"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/link"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/link"
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${ENGINE_SRC_DIR}/*.h" "${ENGINE_SRC_DIR}/*.hpp")
foreach(HEADER ${HEADER_FILES})
    file(RELATIVE_PATH REL_PATH "${ENGINE_SRC_DIR}" "${HEADER}")

    set(DEST "${ENGINE_EXPORT_DIR}/${REL_PATH}")

    get_filename_component(DEST_DIR "${DEST}" DIRECTORY)

    add_custom_command(
            TARGET enginend PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${HEADER}" "${DEST}"
    )
endforeach()


add_executable(test test.cpp)

set_target_properties(test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/engine/test"
)
target_link_directories(
        test PUBLIC
        "${CMAKE_SOURCE_DIR}/link")
target_link_libraries(test PRIVATE
        "${CMAKE_SOURCE_DIR}/link/libenginend.so"
        "${CMAKE_SOURCE_DIR}/link/libdesktopraylib.so"
)
target_include_directories(test PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/raylib/linux-64/raylib/include
)
target_include_directories(enginend PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/raylib/linux-64/raylib/include

)
