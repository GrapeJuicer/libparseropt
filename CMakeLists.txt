cmake_minimum_required(VERSION 3.16)
project(libparseropt VERSION 1.0.0)

add_library(parseropt)

target_include_directories(parseropt
    PUBLIC include
)

target_sources(parseropt
    PRIVATE
        src/parseropt.c
        src/help.c
)
