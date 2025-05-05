<h1 align = center>Vox</h1>
<p align = center>A voxel based game project with custom editor tools</p>

## About The Project
Vox is a simple game that allows players to build, platform, and fight enemies in a voxel-based world. It is currently a work-in-progress! The game uses a custom engine with SDL3 and using the openGL graphics API.

## Features
- Realtime GPU voxel mesh construction
- GLTF mesh skinning with skeletal mesh animation support
- PBR material lighting model
- World editor with loading/saving support

## Requirements
This project requires [vcpkg](https://github.com/microsoft/vcpkg) and cmake to build.

## Dependencies
- [SDL3](https://github.com/libsdl-org/SDL) windows, event handling, and system IO
- [GLEW](https://github.com/nigels-com/glew) openGL API bindings
- [Dear ImGui](https://github.com/ocornut/imgui) editor tools
- [Jolt](https://github.com/jrouwe/JoltPhysics) physics
- [nlohmann-json](https://github.com/nlohmann/json) json parsing
- [tiny-gltf](https://github.com/syoyo/tinygltf) gltf file parsing
