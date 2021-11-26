cmake_minimum_required(VERSION 3.1)
project(starrider)

set(CMAKE_CXX_STANDARD 20)
set(starrider_version 0.1)

set(PROJECT_VERSION ${starrider_version})
project(${PROJECT_NAME} VERSION ${starrider_version} LANGUAGES CXX C)

#file(GLOB SOURCES "${PORJECT_SOURCE_DIR}/main.cpp" "${PROJECT_SOURCE_DIR}/src/*.cpp")

set(SOURCES src/main.cpp src/graphics/Window.cpp src/graphics/Renderer.cpp src/graphics/Core.cpp src/graphics/Rasterizer.cpp src/io/BmpReader.cpp)
set(HEADERS include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED")

include_directories(src)
set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER})
target_link_libraries(${PROJECT_NAME} X11 Xext)
