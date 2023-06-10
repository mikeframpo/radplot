
find_package(glfw3 REQUIRED)

add_library(radplot SHARED
    src/Figure.cpp
    src/Log.cpp
    src/Error.cpp
    src/Window.cpp
)

# Public interface headers are defined in the include/ directory
target_include_directories(radplot PUBLIC ./include)

target_link_libraries(radplot PRIVATE glfw)
