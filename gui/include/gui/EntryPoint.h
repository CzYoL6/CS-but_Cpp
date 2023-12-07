#pragma once

#include <windows.h>

#include <gui/SteamworksHelper.h>

extern GGgui::Application* GGgui::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace GGgui {

	int Main(int argc, char** argv)
	{


		return 0;
	}

}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    RealMain([&](){
        while (g_ApplicationRunning)
        {
            GGgui::Application* app = GGgui::CreateApplication(__argc, __argv);
            app->Run();
            delete app;
        }
    });
	return 0;
}

//int main(int argc, char** argv)
//{
//	return GGgui::Main(argc, argv);
//}
