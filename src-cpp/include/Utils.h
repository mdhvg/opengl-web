#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <GLFW/glfw3.h>

template <class T, class P>
T max(T a, P b);

template <class T, class P, class... Args>
T max(T a, P b, Args... args);

template <class T, class P>
T min(T a, P b);

template <class T, class P, class... Args>
T min(T a, P b, Args... args);

GLFWwindow* createWindow(int width, int height, const char* title);

#endif // UTILS_H