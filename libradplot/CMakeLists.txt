
add_library(radplot SHARED
    src/Radplot.cpp
)

# Public interface headers are defined in the include/ directory
target_include_directories(radplot PUBLIC ./include)
