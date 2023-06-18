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

    // Allow move assignment
    GLBuffer& operator=(GLBuffer&&);

    ~GLBuffer();

    void Bind() const;
    void Unbind() const;

protected:
    // Default implementation, no buffer allocated.
    GLBuffer();

    GLBuffer(const void* data, size_t size, VBDataUsage usage, unsigned int gl_buffertype);

    unsigned int _buffer_id;
    unsigned int _buffer_type;
};

class VertexBuffer : public GLBuffer {
public:
    VertexBuffer();
    VertexBuffer(const void* data, size_t count, VBDataUsage usage);

    void SetData(void* data, size_t count);
};

class IndexBuffer : public GLBuffer {
public:
    using IndexT = unsigned int;
    IndexBuffer();
    IndexBuffer(const std::vector<IndexT>& indices);
};

class VertexLayout {
public:
    struct AttrLayout {
        unsigned int GLType;
        int AttrCount;
        size_t Stride;
        size_t Offset;
    };
    using AttrLayoutVec = std::vector<AttrLayout>;

    void PushFloatAttr(int count, size_t stride, size_t vertex_offset);

    const AttrLayoutVec& GetLayout() const { return _attrs; }
private:
    AttrLayoutVec _attrs;
};

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray& other) = delete;
    VertexArray(VertexArray&& other);

    VertexArray& operator=(VertexArray&& other);

    static VertexArray CreateArray(VertexBuffer&& buffer, const VertexLayout& layout);

    VertexBuffer& GetVertexBuffer() { return _buffer; }

    void Bind() const;
    void Unbind() const;
private:
    void ApplyLayout(const VertexLayout& layout);

    unsigned int _vao;
    unsigned int _nattrib;
    VertexBuffer _buffer;
};

}  // namespace radplot
