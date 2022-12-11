cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(vrend VERSION 1.0)
add_executable(vrend 
	thirdparty/glad/glad.c
	thirdparty/libtinyfiledialogs/tinyfiledialogs.c
	thirdparty/imgui/imgui/backends/imgui_impl_glfw.cpp
	thirdparty/imgui/imgui/backends/imgui_impl_opengl3.cpp
	thirdparty/imgui/imgui/imgui.cpp
	thirdparty/imgui/imgui/imgui_draw.cpp
	thirdparty/imgui/imgui/imgui_tables.cpp
	thirdparty/imgui/imgui/imgui_widgets.cpp
	thirdparty/ImGradientHDR/src/ImGradientHDR.cpp
	src/main.cpp
	src/MainApplication.cpp
	src/Graphics/Shader.cpp
	src/Graphics/Texture.cpp
	src/IO/MouseButtonState.cpp
	src/Model/TransferFunction1D.cpp
	src/Model/VolumeDataset.cpp
	src/UI/VolumeViewWindow.cpp
	src/UI/VolumeViewWindow2D.cpp
	src/UI/VolumeViewWindow3D.cpp
)

add_definitions(-D_DEBUG)

add_subdirectory(thirdparty/glfw)
include_directories(thirdparty/glfw/include)
target_link_libraries(vrend glfw ${GLFW_LIBRARIES})


include_directories(thirdparty/glad)
include_directories(thirdparty/libtinyfiledialogs)
include_directories(thirdparty/glm)
include_directories(thirdparty/ImGradientHDR/src)
include_directories(thirdparty/imgui)
include_directories(thirdparty/imgui/imgui)
include_directories(thirdparty/imgui/imgui/backends)

