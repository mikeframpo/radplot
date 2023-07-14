#pragma once

// TODO: enable PCH maybe, include a bunch of std headers

#include "Error.h"
#include "Window.h"
#include "FigurePriv.h"
#include "Log.h"
#include "Renderer.h"
#include "Buffer.h"
#include "Shader.h"

#include "radplot/Figure.h"
#include "radplot/Math.h"

// TODO: selectively include glm headers?
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <array>
#include <iostream>
#include <optional>
#include <algorithm>

#ifndef NDEBUG
#define DEBUG_ASSERT(condition) \
    do                                                                                  \
    {                                                                                   \
        if (!(condition))                                                               \
        {                                                                               \
            std::cerr << std::format("ASSERT {}:{} {}\n", __FILE__, __LINE__, #condition);  \
            std::terminate();                                                           \
        }                                                                               \
    }                                                                                   \
    while (0);
#endif