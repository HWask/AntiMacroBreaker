#include "CaptureImage.h"

bool AntiMacroLib::CCaptureImage::IsWindowAlive(char* window) {
	if (FindWindow(NULL, window) != NULL)
		return true;
	else
		return false;
}

std::vector<char> AntiMacroLib::CCaptureImage::CaptureImage(char* window) {
	auto hwNd = FindWindow(NULL, window);
	if (!hwNd)
		return std::vector<char>();

	HDC hdcScreen;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	hdcScreen = GetDC(hwNd);

	hdcMemDC = CreateCompatibleDC(hdcScreen);
	if (!hdcMemDC)
	{
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return std::vector<char>();
	}

	RECT rect;
	GetClientRect(hwNd, &rect);

	hbmScreen = CreateCompatibleBitmap(hdcScreen, rect.right - rect.left, rect.bottom - rect.top);
	if (!hbmScreen)
	{
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return std::vector<char>();
	}

	SelectObject(hdcMemDC, hbmScreen);

	if (!BitBlt(hdcMemDC,
		0, 0,
		rect.right - rect.left, rect.bottom - rect.top,
		hdcScreen,
		0, 0,
		SRCCOPY))
	{
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return std::vector<char>();
	}

	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	GetDIBits(hdcMemDC, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42; //BM   

	char* image = new char[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwBmpSize];
	memcpy(image, &bmfHeader, sizeof(BITMAPFILEHEADER));
	memcpy(image + sizeof(BITMAPFILEHEADER), &bi, sizeof(BITMAPINFOHEADER));
	memcpy(image + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), lpbitmap, dwBmpSize);

	std::string buff(image, sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwBmpSize);
	std::vector<char> data(buff.begin(), buff.end());
	delete[] image;

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);

	return data;
}