#include "imguiWin.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <SDL.h>

#pragma comment(lib,"SDL2")
#pragma comment(lib,"opengl32")

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
#include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <string>

bool ingui_wnd_render(SDL_Window* window, ImGuiIO& io);

#pragma message("한글처리 부분 임시로 주석처리")
/* jketst
//
// convert_utf8_to_unicode_string
//
DWORD convert_utf8_to_unicode_string(
	__out std::wstring& unicode,
	__in const char* utf8,
	__in const size_t utf8_size
) {
	DWORD error = 0;
	do {
		if ((nullptr == utf8) || (0 == utf8_size)) {
			error = ERROR_INVALID_PARAMETER;
			break;
		}
		unicode.clear();
		//
		// getting required cch.
		//
		int required_cch = ::MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			utf8, static_cast<int>(utf8_size),
			nullptr, 0
		);
		if (0 == required_cch) {
			error = ::GetLastError();
			break;
		}
		//
		// allocate.
		//
		unicode.resize(required_cch);
		//
		// convert.
		//
		if (0 == ::MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			utf8, static_cast<int>(utf8_size),
			const_cast<wchar_t*>(unicode.c_str()), static_cast<int>(unicode.size())
		)) {
			error = ::GetLastError();
			break;
		}
	} while (false);
	return error;
}

char szComposite[10] = { 0, };

// 한글입력 조합중인 문자 화면에 출력
void show_composition_string(const char* text) {
	if (!text)
		return;

	ImGuiContext* ctx = ImGui::GetCurrentContext();
	if (!ctx)
		return;

	// if doesn't textinput focused
	if (ctx->WantTextInputNextFrame == -1)
		return;

	ImGuiIO& io = ImGui::GetIO();

	std::wstring wstr;
	convert_utf8_to_unicode_string(wstr, text, strlen(text));
	OutputDebugStringW(wstr.c_str());

	static ImVec2 window_pos = ImVec2();
	window_pos = ImVec2(ImGui::GetCurrentContext()->PlatformImePos.x + 1.0f, ImGui::GetCurrentContext()->PlatformImePos.y); // 

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_Tooltip;

	if (ImGui::Begin("IME Composition Window", nullptr, flags))
		ImGui::Text(text);
	ImGui::End();
	ImGui::PopStyleVar();
}
*/

class imguiWnd {
public:
	imguiWin* _obj;
	SDL_Window* _sdlWnd;
public:
	imguiWnd():_sdlWnd(nullptr), _obj(nullptr) {};

	int main(imguiWin* pObj, const char* title, WNDCLASSEX* pWcex); 

	bool render(ImGuiIO& io);
private:
	void setError(const char* message);
	HWND createWindow(const char* title, WNDCLASSEX* pWcex);
	
};

imguiWnd gInstance;


// Our state


int imguiWnd::main(imguiWin* pObj, const char* title, WNDCLASSEX* pWcex) {
	if (!pObj)
		return -1;

	_obj = pObj;

	HWND hWnd = createWindow(title, pWcex);
	if(!hWnd) {
		setError("Failed to createWindow");
		return -1;
	}
		   
	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		setError(SDL_GetError());
		return -1;
	}

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GLContext gl_context = nullptr;
	// child window로 생성할 때
	{
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
		SDL_Window* pSampleWin = SDL_CreateWindow(nullptr, 0, 0, 0, 0, window_flags);

		char buf[32];
		sprintf_s<32>(buf, "%p", pSampleWin);
		SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, buf);
		_sdlWnd = SDL_CreateWindowFrom((const void*)hWnd);
		SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, nullptr);
		gl_context = SDL_GL_CreateContext(_sdlWnd);
		SDL_GL_MakeCurrent(_sdlWnd, gl_context);
		SDL_GL_SetSwapInterval(1); // Enable vsync
	}
	// 일반적인 방식으로 생성하는 방법은 기존 imgui example에서 확인


	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err) {
		setError("Failed to initialize OpenGL loader!");
		return -1;
	}
	
	// Setup Dear ImGui context
	ImGuiContext* ctx = _obj->onCreate(hWnd);
	if(!ctx)
		return -1;

	ImGui::SetCurrentContext(ctx);
	
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(_sdlWnd, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGuiIO& io = ImGui::GetIO();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
#pragma message("****************** 폰트설정부분은 나중에 반드시")
	//jktest io.Fonts->AddFontFromFileTTF("./fonts/GothicA1-Regular.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	MSG msg = { 0, };

	// Main loop
	bool done = false;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(_sdlWnd))
				done = true;

#pragma message("한글처리 임시로 주석처리 함.")
			/* jktest 
			else if (event.type == SDL_TEXTEDITING) {
				strcpy_s(szComposite, event.text.text);
			}
			else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
					if (strlen(szComposite) > 0) {
						io.KeyMap[ImGuiKey_Backspace] = -1;// SDL_SCANCODE_BACKSPACE;
					}
					else {
						io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
					}
				}
			}
			*/
		}

		if(!render(io))
			break;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	_obj->onDestroy();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(_sdlWnd);
	SDL_Quit();
	return 0;
	

	return -1;
}

void imguiWnd::setError(const char* message) {
	if (!message)
		return;
	//m_pObj->onError(message);
	OutputDebugStringA(message);
}

bool imguiWnd::render(ImGuiIO& io) {
	
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(_sdlWnd);
	
	ImDrawData* pDrawData = _obj->onRender();
	if(!pDrawData)
		return false;

	const ImVec4& clr = _obj->getClearColor().Value;

	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clr.x, clr.y, clr.z, clr.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(pDrawData);
	SDL_GL_SwapWindow(_sdlWnd);
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch(msg) 
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_WINDOWPOSCHANGED:
		if(ImGui::GetCurrentContext())
			gInstance.render(ImGui::GetIO());
		break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}

HWND imguiWnd::createWindow(const char* title, WNDCLASSEX* pWcex) {

	WNDCLASSEX wcex = {0,};

	if(!pWcex) {		
		pWcex = &wcex;

		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW|CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.hInstance		= (HINSTANCE)GetModuleHandle(nullptr);
		wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszClassName	= "imguiWin";
	}
	
	RegisterClassEx(pWcex);

	if(!title)
		title = "imguiWin";
	HWND hWnd = CreateWindow(wcex.lpszClassName, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, wcex.hInstance, nullptr);
	if(!hWnd)
		return nullptr;
	
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	return hWnd;
}

int imguiWinMain(imguiWin* obj, const char* title, WNDCLASSEX* pWcex) {
	if (!obj)
		return -1;
	return gInstance.main(obj, title, pWcex);
}

