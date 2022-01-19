
add_library(math "")

target_sources(math
    PRIVATE
    Core.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Core.h
)

target_include_directories(math
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
