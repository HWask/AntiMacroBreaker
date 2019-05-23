#include "AntiMacroCLR.h"

AntiMacroCLR::MacroBreaker::MacroBreaker() {

}

void AntiMacroCLR::MacroBreaker::SimulateMouseClick(String^ windowName, Point p) {
	IntPtr str = Runtime::InteropServices::Marshal::StringToHGlobalAnsi(windowName);
	std::string channel = std::string(reinterpret_cast<char*>(str.ToPointer()));

	AntiMacroLib::MouseInput::SimulateMouseClick((char*)channel.c_str(), p.X, p.Y);

	Runtime::InteropServices::Marshal::FreeHGlobal(str);
}

Bitmap^ AntiMacroCLR::MacroBreaker::CaptureImage(String^ windowName) {
	IntPtr str = Runtime::InteropServices::Marshal::StringToHGlobalAnsi(windowName);
	std::string channel = std::string(reinterpret_cast<char*>(str.ToPointer()));

	auto data = AntiMacroLib::CCaptureImage::CaptureImage((char*)channel.c_str());

	Runtime::InteropServices::Marshal::FreeHGlobal(str);

	array<Byte>^ buffer = gcnew array<Byte>(data.size());
	for (int i = 0; i < data.size(); i++) {
		buffer[i] = data[i];
	}

	ImageConverter^ converter = gcnew ImageConverter();
	Image^ image = (Image^)converter->ConvertFrom(buffer);
	Bitmap^ bitmap = gcnew Bitmap(image);

	return bitmap;
}

bool AntiMacroCLR::MacroBreaker::IsWindowAlive(String^ windowName) {
	IntPtr str = Runtime::InteropServices::Marshal::StringToHGlobalAnsi(windowName);
	std::string channel = std::string(reinterpret_cast<char*>(str.ToPointer()));

	bool ret = AntiMacroLib::CCaptureImage::IsWindowAlive((char*)channel.c_str());
	Runtime::InteropServices::Marshal::FreeHGlobal(str);

	return ret;
}

bool AntiMacroCLR::MacroBreaker::IsMacroPopped(Bitmap^ image) {
	MemoryStream^ stream = gcnew MemoryStream();
	image->Save(stream, Imaging::ImageFormat::Bmp);
	array<Byte>^ buffer = stream->ToArray();

	std::vector<char> data;
	data.resize(buffer->Length);

	for (int i = 0; i < buffer->Length; i++) {
		data[i] = buffer[i];
	}

	AntiMacroLib::MacroSolver solver(data);
	if (solver.IsMacroPopped())
		return true;
	else
		return false;
}

List<Point>^ AntiMacroCLR::MacroBreaker::Solve(Bitmap^ image) {
	MemoryStream^ stream = gcnew MemoryStream();
	image->Save(stream, Imaging::ImageFormat::Bmp);
	array<Byte>^ buffer = stream->ToArray();

	std::vector<char> data;
	data.resize(buffer->Length);

	for (int i = 0; i < buffer->Length; i++) {
		data[i] = buffer[i];
	}

	List<Point>^ points = gcnew List<Point>();
	AntiMacroLib::MacroSolver solver(data);
	auto vec = solver.Solve();

	for (int i = 0; i < vec.size(); i++) {
		points->Add(Point(vec[i].x, vec[i].y));
	}

	return points;
}