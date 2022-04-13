cmake_minimum_required(VERSION 3.18)
project(md_file_converter)

set(CMAKE_CXX_STANDARD 20)
set(md_file_converter_version 0.1)

set(PROJECT_VERSION ${md_file_converter_version})
project(${PROJECT_NAME} VERSION ${md_file_converter_version} LANGUAGES CXX C)

set(SOURCES
    main.cpp
)

set(HEADERS include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wextra -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED")

include_directories(

)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER})
