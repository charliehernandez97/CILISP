cmake_minimum_required(VERSION 3.8)

project(cilisp)

SET(CMAKE_C_FLAGS "-m64 -g -O0 -D_DEBUG -Wall")

set(
        SOURCE_FILES
        src/ciLisp.c
        ${CMAKE_SOURCE_DIR}/src/bison-flex-output/lexer.c
        ${CMAKE_SOURCE_DIR}/src/bison-flex-output/parser.c
)

include_directories(AFTER src ${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON)
find_package(FLEX)

FLEX_TARGET(lexer src/cilisp.l ${CMAKE_SOURCE_DIR}/src/bison-flex-output/lexer.c COMPILE_FLAGS)
BISON_TARGET(parser src/cilisp.y ${CMAKE_SOURCE_DIR}/src/bison-flex-output/parser.c VERBOSE COMPILE_FLAGS)

ADD_FLEX_BISON_DEPENDENCY(lexer parser)

add_executable(
        cilisp
        ${SOURCE_FILES}
        ${BISON_parser_OUTPUTS}
        ${FLEX_lexer_OUTPUTS}
)

set_property(TARGET cilisp PROPERTY C_STANDARD 11)
set_property(TARGET cilisp PROPERTY C_STANDARD_REQUIRED ON)

target_include_directories(cilisp PRIVATE ${CMAKE_SOURCE_DIR}/src/bison-flex-output/)

target_sources(cilisp PRIVATE src/ciLisp.c src/ciLisp.h)
target_sources(cilisp PRIVATE ${BISON_parser_OUTPUTS})
target_sources(cilisp PRIVATE ${FLEX_lexer_OUTPUTS})

target_link_libraries(cilisp m)
