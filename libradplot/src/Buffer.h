#pragma once

#include <stddef.h>
#include <vector>

namespace radplot {

// Don't want to include GL in header file - too sensitive to include ordering fuck ups
enum class VBDataUsage : int {
    StaticDraw = 0x88E4,  // GL_STATIC_DRAW
};

class GLBuffer {
public:
    // Buffers can be moved but not copy-constructed or copy-assigned.
    GLBuffer(const GLBuffer& other) = delete;
    GLBuffer(GLBuffer&& other);

    ~GLBuffer();

    void Bind() const;
    void Unbind() const;

protected:
    GLBuffer(void* data, size_t size, VBDataUsage usage, unsigned int gl_buffertype);

private:
    unsigned int _bufferID;
    unsigned int _buffer_type;
};

class VertexBuffer : public GLBuffer {
public:
    VertexBuffer(void* data, size_t count, VBDataUsage usage);
};

class IndexBuffer : public GLBuffer {
public:
    using IndexT = unsigned int;
    IndexBuffer(std::vector<IndexT> indices, size_t count);

    size_t Count() const;

private:
    size_t _count;
};

}  // namespace radplot
