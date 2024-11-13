#pragma once

#include "tinyfiledialogs.h"

#include <functional>
#include <string>

class Editor
{
public:
	void Draw();
	void BindOnGLTFOpened(std::function<void(std::string)> function);

private:
	void DrawToolbar();
	void DrawFileToolbar();
	void DrawImportToolbar();

	void openGLTF();

	std::function<void(std::string)> onGLTFOpened = [](std::string string) {};

	static const char* gltfFilter[2];
};