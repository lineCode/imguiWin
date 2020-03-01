#ifndef _imgui_win_h_
#define _imgui_win_h_
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <imgui.h>

#if defined(IMGUIWIN32_EXPORTS)
#	define IMGUIWIN_EXPORT extern "C" __declspec(dllexport)
#else
#	define IMGUIWIN_EXPORT extern "C" __declspec(dllimport)
#endif

struct imguiWin {
	virtual ~imguiWin() {}
	imguiWin():_clearColor(0.45f, 0.55f, 0.60f) {}

	virtual ImGuiContext*	onCreate(HWND hWnd) = 0;
	virtual ImDrawData*		onRender() = 0;	
	virtual void			onDestroy() = 0;
		
	inline int run(const char* title, WNDCLASSEX* wcex=nullptr);

	virtual void			setClearColor(ImColor& color) { _clearColor = color; }
	virtual const ImColor&	getClearColor() const { return _clearColor; }

private:
	ImColor _clearColor;
};

IMGUIWIN_EXPORT int imguiWinMain(imguiWin* obj, const char* title, WNDCLASSEX* wcex);

inline int imguiWin::run(const char* title, WNDCLASSEX* wcex/* =nullptr */) {
	return imguiWinMain(this, title, wcex);
}

#endif // _imgui_win_h_
