#include "Buffer.h"

#include <GL/glew.h>

namespace radplot {
GLBuffer::GLBuffer() : _buffer_id(0), _buffer_type(0) {}

GLBuffer::GLBuffer(const void* data, size_t size, VBDataUsage usage, unsigned int gl_buffertype)
    : _buffer_id(0), _buffer_type(gl_buffertype) {

    glGenBuffers(1, &_buffer_id);
    Bind();
    glBufferData(_buffer_type, size, data, static_cast<GLenum>(usage));
    Unbind();
}

GLBuffer& GLBuffer::operator=(GLBuffer&& other) {
    _buffer_id = other._buffer_id;
    _buffer_type = other._buffer_type;
    
    // Set other to 0 so that it's not freed
    other._buffer_id = 0;

    return *this;
}

GLBuffer::GLBuffer(GLBuffer&& other) {
    _buffer_id = other._buffer_id;
    other._buffer_id = 0;

    _buffer_type = other._buffer_type;
    other._buffer_type = 0;
}

GLBuffer::~GLBuffer() {
    if (_buffer_id != 0) {
        glDeleteBuffers(1, &_buffer_id);
        _buffer_id = 0;
    }
}

void GLBuffer::Bind() const {
    if (_buffer_id != 0)
        glBindBuffer(_buffer_type, _buffer_id);
}

void GLBuffer::Unbind() const { glBindBuffer(_buffer_type, 0); }

VertexBuffer::VertexBuffer() : GLBuffer() {}

VertexBuffer::VertexBuffer(const void* data, size_t count, VBDataUsage usage)
    : GLBuffer(data, count, usage, GL_ARRAY_BUFFER) {}

void VertexBuffer::SetData(void* data, size_t count) {
    //TODO: store the usage in the buffer?
    Bind();
    glBufferData(_buffer_type, count, data, GL_STATIC_DRAW);
    Unbind();
}

IndexBuffer::IndexBuffer(const std::vector<IndexT>& indices)
    : GLBuffer(indices.data(), indices.size() * sizeof(IndexT), VBDataUsage::StaticDraw, GL_ELEMENT_ARRAY_BUFFER) {}

IndexBuffer::IndexBuffer() : GLBuffer() {}

void VertexLayout::PushFloatAttr(int count, size_t stride, size_t vertex_offset) {
    _attrs.push_back({GL_FLOAT, count, stride, vertex_offset});
}

VertexArray::VertexArray() : _vao(0), _nattrib(0), _buffer() { glGenVertexArrays(1, &_vao); }

VertexArray::~VertexArray() {
    if (_vao != 0) {
        glDeleteVertexArrays(1, &_vao);
    }
}

void VertexArray::SetVertexBuffer(VertexBuffer&& buffer, const VertexLayout& layout) {
    Bind();
    _buffer = std::move(buffer);
    _buffer.Bind();

    for (auto& attr : layout.GetLayout()) {
        glVertexAttribPointer(_nattrib, attr.AttrCount, attr.GLType, GL_FALSE, attr.Stride,
                              reinterpret_cast<void*>(attr.Offset));
        glEnableVertexAttribArray(_nattrib);

        _nattrib++;
    }

    _buffer.Unbind();
    Unbind();
}

void VertexArray::Bind() const { glBindVertexArray(_vao); }

void VertexArray::Unbind() const { glBindVertexArray(0); }

}  // namespace radplot
