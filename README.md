## Example

```c++
#include "imguiWin.h"

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
```



![](https://github.com/smok95/imguiWin/blob/master/screenshot.png)
