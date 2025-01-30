#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"