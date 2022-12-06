#include "FileDialog.h"
#include <Windows.h>
#include <commdlg.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace vr
{
    std::string OpenFileDialog(GLFWwindow* window, const char* filter)
    {
        CHAR fileStrBuf[512] = { 0 };

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize  = sizeof(OPENFILENAME);
        ofn.hwndOwner    = glfwGetWin32Window(window);
        ofn.lpstrFile    = fileStrBuf;
        ofn.nMaxFile     = sizeof(fileStrBuf);
        ofn.lpstrFilter  = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        
        if (GetOpenFileNameA(&ofn) == TRUE)
            return ofn.lpstrFile;
        return "";
    }
}