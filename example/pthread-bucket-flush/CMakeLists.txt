PROJECT(chatnetcli)
CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/deps)

ADD_LIBRARY(lib-main SHARED lib.c)
ADD_LIBRARY(lib-bucket-flush SHARED bucket-flush.c)
ADD_LIBRARY(linenoise SHARED deps/linenoise/linenoise.c deps/linenoise/linenoise.c)

INCLUDE(ExternalProject)

# <NOTE> USE LOCAL LINENOISE INSTEAD!
#EXTERNALPROJECT_ADD(
  #linenoise
  #PREFIX linenoise
  #URL https://github.com/antirez/linenoise/archive/refs/tags/1.0.tar.gz
  #BUILD_IN_SOURCE 1
  #CONFIGURE_COMMAND ""
  #BUILD_COMMAND ""
  #INSTALL_COMMAND ""
  #TEST_COMMAND ""
  #LOG_DOWNLOAD ON
#)
EXTERNALPROJECT_ADD(
  cmark
  PREFIX cmark
  URL https://github.com/commonmark/cmark/archive/refs/tags/0.30.3.tar.gz
  BUILD_ALWAYS OFF
  LOG_DOWNLOAD ON
  INSTALL_COMMAND ""
)

ADD_EXECUTABLE(chatnetcli main.c)
ADD_DEPENDENCIES(linenoise cmark)
TARGET_LINK_LIBRARIES(chatnetcli linenoise)
TARGET_COMPILE_OPTIONS(chatnetcli PRIVATE -Wall -Wextra -fsanitize=address)
TARGET_LINK_OPTIONS(chatnetcli PRIVATE -fsanitize=address)
