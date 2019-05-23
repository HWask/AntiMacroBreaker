#pragma once
#include <Windows.h>

namespace AntiMacroLib {
	class MouseInput {
	private:
		MouseInput();
		static int mapIntervalLinearly(int a, int b, int c, int d, int x);
	public:
		static void SimulateMouseClick(char* windowName, int x, int y);
	};
}