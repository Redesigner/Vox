# CMakeList.txt : CMake project for Voxels2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.19.0)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Voxels")

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

include(FetchContent)
FetchContent_Declare(
	raylib-sdl
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
)
set(PLATFORM SDL)
FetchContent_MakeAvailable(raylib-sdl)
unset(PLATFORM)

file (GLOB_RECURSE Voxels_SRC
	"src/*.cpp"
)

# Add source to this project's executable.
add_executable (Voxels ${Voxels_SRC})

target_link_libraries(Voxels PRIVATE raylib)
target_include_directories(Voxels PRIVATE "${PROJECT_SOURCE_DIR}/raylib/raylib/include")

# Add Raylib ImGui
target_include_directories(Voxels PRIVATE "external/rlImGui")
target_sources(Voxels PRIVATE
	"external/rlImGui/rlImGui.cpp"
)

# Add ImGui
target_include_directories(Voxels PRIVATE "external/imgui")
target_sources(Voxels PRIVATE
	"external/imgui/imgui.h"
	"external/imgui/imgui.cpp"
	"external/imgui/imgui_demo.cpp"
	"external/imgui/imgui_draw.cpp"
	"external/imgui/imgui_tables.cpp"
	"external/imgui/imgui_widgets.cpp"
)

set_property(TARGET Voxels PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
