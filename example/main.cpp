#include "imguiWin.h"

#if defined(_WIN64)
#	ifdef _DEBUG
#		pragma comment(lib,"imguiWin-x64-Debug")
#	else
#		pragma comment(lib,"imguiWin-x64-Release")
#	endif
#elif defined(_WIN32)
#	ifdef _DEBUG
#		pragma comment(lib,"imguiWin-x86-Debug")
#	else
#		pragma comment(lib,"imguiWin-x86-Release")
#	endif
#endif 

class application :public imguiWin {
	 
    virtual ImGuiContext* onCreate(HWND hwnd) {
		IMGUI_CHECKVERSION();
		ImGuiContext* ctx = ImGui::CreateContext();		 
		return ctx;
	}

    virtual ImDrawData* onRender() {
		ImGui::NewFrame();
		
		ImGui::Begin("imguiWin example");
		ImGui::Text("Hello world!");
		ImGui::End();

		ImGui::Render();
		return ImGui::GetDrawData();
	}

	virtual void onDestroy() {
		ImGui::DestroyContext();
	}
};

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	application app;
	app.run("imguiWin example");
}