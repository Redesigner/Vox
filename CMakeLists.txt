# CMakeList.txt : CMake project for VoxelsRL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.19.0)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Vox")

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
include(FetchContent)

# Build Jolt Physics
FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.2.0"
		SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)
set_property(TARGET Jolt PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Build SDL Image
list(APPEND CMAKE_PROGRAM_PATH "C:\\Program Files\\NASM")
FetchContent_Declare(
	SDLImage
	GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image"
	GIT_TAG "preview-3.1.0"
)
set(SDLIMAGE_AVIF OFF)
FETCHCONTENT_MAKEAVAILABLE(SDLImage)

find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# Add source to this project's executable.
file (GLOB_RECURSE Vox_SRC
	"src/*.cpp"
)

add_executable (Vox ${Vox_SRC})

# Disable RTTI for compatibility with Jolt
string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

# Include source directory to allow absolute paths
target_include_directories(Vox PRIVATE "src/")

target_link_libraries(Vox PRIVATE SDL3::SDL3)
target_link_libraries(Vox PRIVATE SDL3_image::SDL3_image)
target_link_libraries(Vox PRIVATE glm::glm)
target_link_libraries(Vox PRIVATE GLEW::GLEW)
target_link_libraries(Vox PRIVATE tinyfiledialogs::tinyfiledialogs)
target_link_libraries(Vox PRIVATE fmt::fmt)
target_link_libraries(Vox PRIVATE Jolt)

# Add Jolt Physics
target_include_directories(Vox PUBLIC ${JoltPhysics_SOURCE_DIR}/..)

# Add ImGui
target_include_directories(Vox PRIVATE "external")
target_sources(Vox PRIVATE
	"external/imgui/imgui.h"
	"external/imgui/imgui.cpp"
	"external/imgui/imgui_impl_opengl3.cpp"
	"external/imgui/imgui_impl_sdl3.cpp"
	"external/imgui/imgui_demo.cpp"
	"external/imgui/imgui_draw.cpp"
	"external/imgui/imgui_tables.cpp"
	"external/imgui/imgui_widgets.cpp"
)

set_property(TARGET Vox PROPERTY CXX_STANDARD 20)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Vox)
# TODO: Add tests and install targets if needed.
