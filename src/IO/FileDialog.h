#pragma once

#include <string>
#include <GLFW/glfw3.h>

namespace vr
{
    std::string OpenFileDialog(GLFWwindow* window, const char* filter);
}
