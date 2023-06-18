#pragma once

#include "Error.h"

#include <unordered_map>
#include <glm/glm.hpp>
#include <optional>

namespace radplot {

enum class ShaderType {
    Vertex,
    Fragment
};

class Shader {
public:
    static Shader CreateFromStr(const std::string& src, ShaderType type);

    Shader();
    Shader(Shader&& other);
    ~Shader();

    unsigned int GetID() const;
    ShaderType GetType() const { return _type; }

private:
    Shader(unsigned int id, ShaderType type);

    static void PrintProgramLog(unsigned int shader);

    unsigned int _shaderID;
    ShaderType _type;
};

class Program {
public:
    static const Program& LoadProgram(const char* name);

    Program();
    Program(Program&& other);

    void SetVertexShader(Shader&& vertex);
    void SetFragmentShader(Shader&& fragment);
    void Compile();

    void SetUniformMat4f(const char* name, const glm::mat4& mat) const;

    void Bind() const;
    void Unbind() const;
    bool IsBound() const;

private:
    unsigned int _programID;
    std::optional<Shader> _vertex;
    std::optional<Shader> _fragment;

    static std::unordered_map<std::string, Program> _program_cache;
};

}  // namespace