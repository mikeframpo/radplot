
#include <GL/glew.h>

#include "Buffer.h"

namespace radplot {

GLBuffer::GLBuffer(void* data, size_t size, VBDataUsage usage, unsigned int gl_buffertype)
    : _bufferID(0), _buffer_type(gl_buffertype) {

    glGenBuffers(1, &_bufferID);
    Bind();
    glBufferData(_buffer_type, size, data, static_cast<GLenum>(usage));
    Unbind();
}

GLBuffer::GLBuffer(GLBuffer&& other) {
    _bufferID = other._bufferID;
    other._bufferID = 0;

    _buffer_type = other._buffer_type;
    other._buffer_type = 0;
}

GLBuffer::~GLBuffer() {
    if (_bufferID != 0) {
        glDeleteBuffers(1, &_bufferID);
    }
}

void GLBuffer::Bind() const { glBindBuffer(_buffer_type, _bufferID); }

void GLBuffer::Unbind() const { glBindBuffer(_buffer_type, 0); }

VertexBuffer::VertexBuffer(void* data, size_t count, VBDataUsage usage)
    : GLBuffer(data, count, usage, GL_ARRAY_BUFFER) {}

IndexBuffer::IndexBuffer(std::vector<IndexT> indices, size_t count)
    : GLBuffer(indices.data(), indices.size() * sizeof(IndexT), VBDataUsage::StaticDraw, GL_ELEMENT_ARRAY_BUFFER),
      _count(count) {}

size_t IndexBuffer::Count() const { return _count; }

}  // namespace radplot