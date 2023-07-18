
add_library(pyradplot SHARED
    src/pyradplot.cpp
)

target_include_directories(pyradplot PUBLIC ./include)

# link to the cpp api
target_link_libraries(pyradplot PUBLIC radplot)