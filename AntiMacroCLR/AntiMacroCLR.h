#pragma once
#include "CaptureImage.h"
#include "MacroSolver.h"
#include "MouseInput.h"

#using <system.drawing.dll>
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Drawing;
using namespace System::IO;

namespace AntiMacroCLR {

	public ref class MacroBreaker
	{
	private:
		MacroBreaker();
	public:
		static Bitmap^ CaptureImage(String^ windowName);
		static bool IsWindowAlive(String^ windowName);

		static bool IsMacroPopped(Bitmap^ image);
		static List<Point>^ Solve(Bitmap^ image);

		static void SimulateMouseClick(String^ windowName, Point p);
	};
}
