#include <iostream>

#include <gui/Application.h>
#include <gui/EntryPoint.h>
#include <GLFW/glfw3.h>

#include <stb_image/stb_image.h>

class ExampleLayer : public GGgui::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Hello");
		ImGui::Button("Button");

        auto& app = GGgui::Application::Get();
        ImGui::Text("FPS: %d", (int)app.GetSmoothedFps());

        ImGui::End();

//		ImGui::ShowDemoWindow();
	}
};

GGgui::Application* GGgui::CreateApplication(int argc, char** argv)
{
	GGgui::ApplicationSpecification spec;
	spec.Name = "GGgui Example";

	auto* app = new GGgui::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});

	return app;
}