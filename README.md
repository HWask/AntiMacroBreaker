# AntiMacroBreaker

Solves the Captcha that keeps popping up to prevent botting in a game called Ashen Empires.
The library is written in C++ and utilizes OpenCV to do some image analysis.
The GUI which is written in C# runs in the background as a taskbar icon and keeps notifying the user whenever a captcha is
detected and solved. To interface between C++ and C#, C++/CLI has been used.
