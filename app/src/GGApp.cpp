#include <iostream>

#include <gui/Application.h>
#include <gui/EntryPoint.h>
#include <GLFW/glfw3.h>

#include <stb_image/stb_image.h>
#include <app/SettingWindow.h>
#include <app/KillEffectWindow.h>


GGgui::Application* GGgui::CreateApplication(int argc, char** argv)
{
	GGgui::ApplicationSpecification spec;
	spec.Name = "CS but ...";

	auto* app = new GGgui::Application(spec);
	app->PushLayer<SettingWindow>();
    app->PushLayer<KillEffectWindow>();

	return app;
}