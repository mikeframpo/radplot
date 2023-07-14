#include "Shader.h"

#include <GL/glew.h>
#include "Radplot.h"

#include <fstream>
#include <sstream>
#include <format>

namespace radplot {

LOG_MODULE(LogModule::Shader);

std::unordered_map<std::string, Program> Program::_program_cache{};

class ShaderFileIterator {
public:
    ShaderFileIterator(std::ifstream &&stream) : _file(std::move(stream)), _buf(), _cur_type() { ReadNext(); }

    operator bool() const { return _cur_type.has_value(); }

    // Get the text for the next shader
    Shader operator*() {
        DEBUG_ASSERT(static_cast<bool>(*this));

        std::string src = _buf.str();
        Shader shader = Shader::CreateFromStr(src, *_cur_type);
        _buf.str("");
        _cur_type.reset();

        ReadNext();

        return shader;
    }

    void ReadNext() {
        auto prev_pos = _file.tellg();

        std::string line;
        while (std::getline(_file, line)) {
            if (_file.bad())
                throw RadException{"IO Error while reading shader\n"};

            bool vertex_header = line.starts_with("// vertex");
            bool fragment_header = line.starts_with("// fragment");

            if (vertex_header || fragment_header) {
                if (_cur_type) {
                    // at the end, roll back a line for the next header
                    _file.seekg(prev_pos);
                    return;
                } else {
                    // at the start
                    _cur_type = vertex_header ? ShaderType::Vertex : ShaderType::Fragment;
                }
            } else {
                if (_cur_type)
                    _buf << line << '\n';
                // else - still in header
            }

            prev_pos = _file.tellg();
        }
    }

private:
    std::ifstream _file;
    std::stringstream _buf;
    std::optional<ShaderType> _cur_type;
};

Shader Shader::CreateFromStr(const std::string &src, ShaderType type) {
    const char *cstr = src.c_str();
    GLint strlen = src.size();

    GLenum glshader = type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

    unsigned int shader = glCreateShader(glshader);
    glShaderSource(shader, 1, &cstr, &strlen);
    glCompileShader(shader);

    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        PrintProgramLog(shader);
        throw RadException{"Error compiling shader."};
    }

    return Shader(shader, type);
}

void Shader::PrintProgramLog(unsigned int shader) {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    // Fetch the program info log
    std::string buf;
    buf.resize(length);

    int len;
    glGetShaderInfoLog(shader, buf.size(), &len, buf.data());

    LOG_DEBUG("Dumping program log");
    std::cout << buf;
}

Shader::Shader() : _shaderID(0) {}

Shader::Shader(Shader &&other) : _shaderID(other._shaderID), _type(other._type) { other._shaderID = 0; }

Shader::~Shader() {
    if (_shaderID != 0) {
        glDeleteShader(_shaderID);
        _shaderID = 0;
    }
}

unsigned int Shader::GetID() const { return _shaderID; }

Shader::Shader(unsigned int id, ShaderType type) : _shaderID(id), _type(type) {}

Program::Program() : _programID(0), _vertex(), _fragment() { _programID = glCreateProgram(); }

const Program &Program::LoadProgram(const char *name) {
    if (!_program_cache.contains(name)) {
        std::string filename = std::string("../../libradplot/shaders/") + name + ".shader";
        std::ifstream file(filename);

        if (!file.is_open())
            throw RadException{std::format("File failed to open: {}\n", filename)};

        LOG_INFO("Loading Shaders: %s", filename.c_str());
        Program program;

        ShaderFileIterator iter(std::move(file));
        while (iter) {
            Shader shader = *iter;
            switch (shader.GetType()) {
                case ShaderType::Vertex:
                    program.SetVertexShader(std::move(shader));
                    break;
                case ShaderType::Fragment:
                    program.SetFragmentShader(std::move(shader));
                    break;
                default:
                    DEBUG_ASSERT(false);
            }
        }

        program.Compile();
        _program_cache.insert(std::make_pair(name, std::move(program)));
    }

    return _program_cache[name];
}

Program::Program(Program &&other)
    : _programID(other._programID), _vertex(std::move(other._vertex)), _fragment(std::move(other._fragment)) {
    other._programID = 0;
}

void Program::SetVertexShader(Shader &&vertex) { _vertex.emplace(std::move(vertex)); }

void Program::SetFragmentShader(Shader &&fragment) { _fragment.emplace(std::move(fragment)); }

void Program::Compile() {
    // TODO: error if either not set

    glAttachShader(_programID, _vertex->GetID());
    glAttachShader(_programID, _fragment->GetID());

    glLinkProgram(_programID);
    glValidateProgram(_programID);

    int status;
    glGetProgramiv(_programID, GL_VALIDATE_STATUS, &status);

    // TODO: print linker errors

    if (status != GL_TRUE) {
        throw RadException{"Failed to vailidate program"};
    }
}

void Program::SetUniformMat4f(const char *name, const glm::mat4 &mat) const {
    DEBUG_ASSERT(IsBound());

    int location = glGetUniformLocation(_programID, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    } else {
        // Shouldn't be trying to set uniforms that aren't in shader. Shader will likely optimise out variables
        // that aren't used, so this is pretty easy to trigger accidentally. Don't want to crash though since it's a
        // pain during development.
        LOG_ERROR("Failed to set uniform %s", name);
    }
}

void Program::Bind() const { glUseProgram(_programID); }

void Program::Unbind() const { glUseProgram(0); }

bool Program::IsBound() const {
    int programID;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

    return programID == _programID;
}

}  // namespace radplot