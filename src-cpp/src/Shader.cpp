#include "Shader.h"
#include "Debug.h"

#include <fstream>
#include <cassert>
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <iostream>

unsigned int compileShader(const char* source, unsigned int type) {
    unsigned int id = glCreateShader(type);
    const char* sourcePtr = source;
    GLCall(glShaderSource(id, 1, &sourcePtr, nullptr));
    GLCall(glCompileShader(id));
    int compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    char infoLog[512];
    if (!compileStatus) {
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        fmt::println("ERROR::SHADER::{}::COMPILATION_FAILED\n{}", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT", infoLog);
        assert(false);
    }
    return id;
}

unsigned int createRenderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int id = glCreateProgram();
    GLCall(glAttachShader(id, vertexShader));
    GLCall(glAttachShader(id, fragmentShader));
    GLCall(glLinkProgram(id));
    int linkStatus;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
    char infoLog[512];
    if (!linkStatus) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        fmt::println("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
        assert(false);
    }
    return id;
}

unsigned int createComputeProgram(unsigned int computeShader) {
    unsigned int id = glCreateProgram();
    GLCall(glAttachShader(id, computeShader));
    GLCall(glLinkProgram(id));
    int linkStatus;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
    char infoLog[512];
    if (!linkStatus) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        fmt::println("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
        assert(false);
    }
    return id;
}