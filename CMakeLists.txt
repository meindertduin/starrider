cmake_minimum_required(VERSION 3.18)
project(starrider)

set(CMAKE_CXX_STANDARD 20)
set(starrider_version 0.1)


set(PROJECT_VERSION ${starrider_version})
project(${PROJECT_NAME} VERSION ${starrider_version} LANGUAGES CXX C)

set(SOURCES
    src/main.cpp
    src/core/Window.cpp
    src/graphics/Renderer.cpp
    src/graphics/Core.cpp
    src/graphics/Rasterizer.cpp
    src/io/BmpReader.cpp
    src/graphics/Camera.cpp
    src/io/ObjReader.cpp
    src/graphics/RenderPipeline.cpp
    src/entity/Entity.cpp
    src/core/Application.cpp
    src/core/Events.cpp
    src/core/Time.cpp
    src/core/Cursor.cpp
    src/graphics/Texture.cpp
    src/graphics/Font.cpp
    src/graphics/XLib.cpp
    src/math/Matrix.cpp
    src/math/Core.cpp
    src/math/Polar.cpp
    src/math/Vector.cpp
)

set(HEADERS include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -O2 -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED")

include_directories(
    src
    /usr/include/freetype2
)

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER})
target_link_libraries(${PROJECT_NAME} X11 Xext freetype)
