#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace AntiMacroLib {
	class CCaptureImage {
	private:
		CCaptureImage();
	public:
		static std::vector<char> CaptureImage(char* window);
		static bool IsWindowAlive(char* window);
	};
}