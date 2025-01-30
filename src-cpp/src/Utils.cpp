#include "Utils.h"
#include "Debug.h"
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


template <class T, class P>
T max(T a, P b) {
    return a > b ? a : b;
}

template <class T, class P, class... Args>
T max(T a, P b, Args... args) {
    return max(max(a, b), args...);
}

template <class T, class P>
T min(T a, P b) {
    return a < b ? a : b;
}

template <class T, class P, class... Args>
T min(T a, P b, Args... args) {
    return min(min(a, b), args...);
}

template int max<int, double>(int, double);
template double max<double, int>(double, int);
template float max<float, int>(float, int);

template int min<int, double>(int, double);
template double min<double, int>(double, int);
template float min<float, int>(float, int);

GLFWwindow* createWindow(int width, int height, const char* title) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return nullptr;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (window == nullptr) {
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    const char* glsl_version = "#version 460 core";

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return window;
}

