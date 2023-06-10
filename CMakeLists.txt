
cmake_minimum_required(VERSION 3.14)

project(
    radplot
    VERSION 0.1
    DESCRIPTION "Really RAD plotting library!"
    LANGUAGES CXX
)

# library code
add_subdirectory(libradplot)

# test app
add_subdirectory(rad)


set_property(TARGET radplot PROPERTY CXX_STANDARD 20)
