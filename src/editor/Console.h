#pragma once
#include "core/logging/Logging.h"

struct ImVec4;

namespace Vox
{
	class Console
	{
	public:
		Console();
		
		void Draw();


	private:
		void DrawTabs();

		LogFilter filter;

		static ImVec4 buttonColorUnselected;
		static ImVec4 buttonColorSelected;
	};
}
