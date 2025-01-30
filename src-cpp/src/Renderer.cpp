#include "Renderer.h"
#include "Debug.h"
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <iostream>

RenderBuffer generateBuffers(
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices,
    std::vector<int>& widths
) {
    RenderBuffer buffer;

    // Generate and bind VAO
    glGenVertexArrays(1, &buffer.vao);
    glBindVertexArray(buffer.vao);

    // Generate and bind VBO
    glGenBuffers(1, &buffer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Generate and bind EBO (AFTER VAO IS BOUND)
    glGenBuffers(1, &buffer.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // Compute stride
    int stride = 0;
    for (int width : widths) {
        stride += width;
    }
    stride *= sizeof(float); // Convert to bytes

    // Set vertex attributes
    int curOffset = 0;
    for (size_t i = 0; i < widths.size(); i++) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            widths[i],
            GL_FLOAT,
            GL_FALSE,
            stride,
            reinterpret_cast<void*>(curOffset) // Corrected offset casting
        );
        curOffset += widths[i] * sizeof(float);
    }

    return buffer;
}