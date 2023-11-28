#pragma once

extern GGgui::Application* GGgui::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace GGgui {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			GGgui::Application* app = GGgui::CreateApplication(argc, argv);
			app->Run();
			delete app;
		}

		return 0;
	}

}

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return GGgui::Main(__argc, __argv);
}

//int main(int argc, char** argv)
//{
//	return GGgui::Main(argc, argv);
//}
