#include "MouseInput.h"

//maps [a,b] -> [c,d], with x in [a,b]
int AntiMacroLib::MouseInput::mapIntervalLinearly(int a, int b, int c, int d, int x) {
	double coeff = (c - d) / (a - b);
	double r = coeff*(x - a) + c;
	return (int)r;
}

//Coordinates are relative to the game window.
void AntiMacroLib::MouseInput::SimulateMouseClick(char* windowName, int x, int y) {
	HWND hwnd = FindWindow(NULL, windowName);
	if (hwnd) {
		POINT p{ x, y };
		ClientToScreen(hwnd, &p);

		int mX = mapIntervalLinearly(0, GetSystemMetrics(SM_CXSCREEN), 0, 65535, p.x);
		int mY = mapIntervalLinearly(0, GetSystemMetrics(SM_CYSCREEN), 0, 65535, p.y);

		INPUT mouse;
		mouse.type = 0;
		mouse.mi.dx = mX;
		mouse.mi.dy = mY;
		mouse.mi.time = 0;
		mouse.mi.mouseData = 0;
		mouse.mi.dwExtraInfo = 0;

		mouse.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse, sizeof(INPUT));

		mouse.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &mouse, sizeof(INPUT));

		mouse.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &mouse, sizeof(INPUT));

	}
}