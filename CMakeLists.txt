# CMakeList.txt : CMake project for VoxelsRL, include source and define
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

# Build raylib with sdl
FetchContent_Declare(
	raylib-sdl
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG 5.5
)
set(PLATFORM SDL)
add_compile_definitions(SUPPORT_CUSTOM_FRAME_CONTROL 1)
FetchContent_MakeAvailable(raylib-sdl)
unset(PLATFORM)

# Build Jolt Physics
FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.2.0"
		SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)
set_property(TARGET Jolt PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# Add source to this project's executable.
file (GLOB_RECURSE Voxels_SRC
	"src/*.cpp"
)

add_executable (Voxels ${Voxels_SRC})

# Disable RTTI for compatibility with Jolt
string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

# Include source directory to allow absolute paths
target_include_directories(Voxels PRIVATE "src/")

target_link_libraries(Voxels PRIVATE raylib)
target_link_libraries(Voxels PRIVATE tinyfiledialogs::tinyfiledialogs)
target_link_libraries(Voxels PRIVATE fmt::fmt)
target_link_libraries(Voxels PRIVATE Jolt)

# Add Raylib ImGui
target_include_directories(Voxels PRIVATE "external/rlImGui")
target_sources(Voxels PRIVATE
	"external/rlImGui/rlImGui.cpp"
)

# Add Jolt Physics
target_include_directories(Voxels PUBLIC ${JoltPhysics_SOURCE_DIR}/..)

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
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Voxels)
# TODO: Add tests and install targets if needed.
