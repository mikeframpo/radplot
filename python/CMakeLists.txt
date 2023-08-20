
add_library(pyradplot SHARED
    src/pyradplot.cpp
)

# link to the cpp api
target_link_libraries(pyradplot PUBLIC radplot)