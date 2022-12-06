#include <iostream>

#include "MainApplication.h"

int main()
{
    try {
        vr::MainApplication app {};
        app.RunMainLoop();
    }
    catch (std::exception& e) {
        std::cerr << "[MainApplication] " << e.what() << '\n';
    }
    return 0;
}