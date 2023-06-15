
add_library(radplot SHARED
    src/Figure.cpp
    src/Log.cpp
    src/Error.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Buffer.cpp
)

# Public interface headers are defined in the include/ directory
target_include_directories(radplot PUBLIC ./include)

target_link_libraries(radplot PUBLIC glfw)
target_link_libraries(radplot PUBLIC GLEW)
target_link_libraries(radplot PUBLIC GL)
