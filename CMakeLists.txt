# cmake setup
project(learning-to-code-in-c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fsanitize=address")


# dependencies
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/pcre/include)
add_library(dep_sc OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/deps/sc/sc_str.c)
add_library(dep_curl SHARED IMPORTED)
set_property(TARGET dep_curl PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcurl.dll)
set_property(TARGET dep_curl PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcurl.dll.a)


# fun project items
add_library(autodeallocate autodeallocate.c)

add_library(lib OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/lib.c)

add_executable(commitcount commitcount.c)
target_link_libraries(commitcount lib dep_sc dep_curl 
${CMAKE_CURRENT_SOURCE_DIR}/lib/libpcre.lib
${CMAKE_CURRENT_SOURCE_DIR}/lib/libpcreposix.lib
${CMAKE_CURRENT_SOURCE_DIR}/lib/jansson.lib)