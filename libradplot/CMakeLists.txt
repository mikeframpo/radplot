
add_library(radplot SHARED
    src/api/Figure.cpp

    src/Log.cpp
    src/Error.cpp
    src/Window.cpp
    src/FigurePriv.cpp
    src/Renderer.cpp
    src/Buffer.cpp
    src/Shader.cpp
)

# Public interface headers are defined in the include/ directory
target_include_directories(radplot PUBLIC ./include)

# add include paths for 3rd party libraries
target_include_directories(radplot PRIVATE ../lib)

target_link_libraries(radplot PUBLIC glfw)
target_link_libraries(radplot PUBLIC GLEW)
target_link_libraries(radplot PUBLIC GL)
