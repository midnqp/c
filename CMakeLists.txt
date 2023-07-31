## cmake setup
project(learning-to-code-in-c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

## dependencies
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/pcre/include)
add_library(dep_sc OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/deps/sc/sc_str.c)
add_library(dep_curl SHARED IMPORTED)
set_property(TARGET dep_curl PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcurl.dll)
set_property(TARGET dep_curl PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcurl.dll.a)
set(dep_all dep_sc dep_curl 
${CMAKE_CURRENT_SOURCE_DIR}/lib/libpcre.lib
${CMAKE_CURRENT_SOURCE_DIR}/lib/libpcreposix.lib
${CMAKE_CURRENT_SOURCE_DIR}/lib/jansson.lib
${CMAKE_CURRENT_SOURCE_DIR}/lib/vld.lib)

## fun project items
add_library(autofree autofree.c)

add_library(lib OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/lib.c)

add_library(commitcount ${CMAKE_CURRENT_SOURCE_DIR}/commitcount.c)
#target_link_libraries(commitcount lib autofree dep_all)

add_library(activeforks ${CMAKE_CURRENT_SOURCE_DIR}/activeforks.c)
#target_link_libraries(activeforks lib autofree dep_all)

add_library(catfacts ${CMAKE_CURRENT_SOURCE_DIR}/catfacts.c)

add_executable(main ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
target_link_libraries(main ${dep_all} autofree lib commitcount activeforks catfacts)