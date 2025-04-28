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

#if (DEFINED $ENV{VCPKG_ROOT})
#	message (STATUS "vcpkg found from env variable")
	include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
#else()
#	message (STATUS "vcpkg not defined in env variables")
#	include (${vcpkg}/scripts/buildsystems/vcpkg.cmake)
#endif()

find_package(fmt CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

add_executable (Vox "")
include ("sources.cmake")

# Disable RTTI for compatibility with Jolt
string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

# Include source directory to allow absolute paths
target_include_directories(Vox PRIVATE "src/")

target_link_libraries(Vox PRIVATE fmt::fmt)
target_link_libraries(Vox PRIVATE GLEW::GLEW)
target_link_libraries(Vox PRIVATE glm::glm)
target_link_libraries(Vox PRIVATE imgui::imgui)
target_link_libraries(Vox PRIVATE Jolt::Jolt)
target_link_libraries(Vox PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(Vox PRIVATE SDL3::SDL3)
target_link_libraries(Vox PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

# Add tinyglTF
target_include_directories(Vox PRIVATE ${TINYGLTF_INCLUDE_DIRS})

set_property(TARGET Vox PROPERTY CXX_STANDARD 20)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Vox)
# TODO: Add tests and install targets if needed.
