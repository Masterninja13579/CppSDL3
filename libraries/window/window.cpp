
#include "window/window.h"

#include <iostream>

using namespace Application;

Window::Window(const char* name)
	: mName(name)
	, mWidth(1280)
	, mHeight(720)
	, mFlags(SDL_WINDOW_RESIZABLE | PLATFORM_SDL_RENDER_FLAG)
	, mIsShown(false)
	, mSDLWindow(nullptr)
{

}

Window::Window(const char* name, int width, int height, SDL_WindowFlags flags)
	: mName(name)
	, mWidth(width)
	, mHeight(height)
	, mFlags(flags)
	, mIsShown(false)
	, mSDLWindow(nullptr)
{

}

void Window::PrintSDLFlags(const SDL_WindowFlags& flags)
{
	std::cout << "Window Flags\n";
	for (auto it = WindowFlagMap.begin(); it != WindowFlagMap.end(); ++it)
	{
		if (flags & it->first)
			std::cout << "   " << it->second << "\n";
	}
}

const char*		Window::GetName() { return mName; }
int				Window::GetWidth() { return mWidth; }
int				Window::GetHeight() { return mHeight; }
SDL_WindowFlags	Window::GetSDLWindowFlags() { return SDL_GetWindowFlags(mSDLWindow); }
SDL_WindowID	Window::GetSDLWindowId() { return SDL_GetWindowID(mSDLWindow); }
bool			Window::IsFullScreen()
{
	Uint64 flags = GetSDLWindowFlags();
	return flags & SDL_WINDOW_FULLSCREEN;
}
bool Window::IsBorderless()
{
	Uint64 flags = GetSDLWindowFlags();
	return flags & SDL_WINDOW_BORDERLESS;
}
bool Window::IsWindowed()
{
	Uint64 flags = GetSDLWindowFlags();
	return !(flags & SDL_WINDOW_BORDERLESS) && !(flags & SDL_WINDOW_FULLSCREEN);
}
bool Window::IsMinimized()
{
	Uint64 flags = GetSDLWindowFlags();
	return flags & SDL_WINDOW_MINIMIZED;
}
bool Window::IsMaximized()
{
	Uint64 flags = GetSDLWindowFlags();
	return flags & SDL_WINDOW_MAXIMIZED;
}


void Window::Create()
{
	if (mIsShown) return;

	InitSDL();

	mSDLWindow = SDL_CreateWindow(mName, mWidth, mHeight, mFlags);
	
	InitBgfx();
	InitImGui();

	mIsShown = true;
}

void Window::Destroy()
{
	if (!mIsShown) return;

	ImGui_ImplSDL3_Shutdown();
	ImGui_Implbgfx_Shutdown();
	ImGui::DestroyContext();
	bgfx::shutdown();
	SDL_DestroyWindow(mSDLWindow);
	SDL_Quit();

	mIsShown = false;
}

void Window::Refresh()
{
	SDL_GetWindowSize(mSDLWindow, &mWidth, &mHeight);
	bgfx::reset(mWidth, mHeight, BGFX_RESET_NONE);
	bgfx::setViewRect(0, 0, 0, mWidth, mHeight);
}

void Window::Resize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	Refresh();
}

void Window::SetFullScreen()
{
	SDL_SetWindowFullscreen(mSDLWindow, true);
}

void Window::SetFullScreen(const SDL_DisplayMode& fullScreenDisplayMode)
{
	SDL_SetWindowBordered(mSDLWindow, false);
	SDL_SetWindowFullscreen(mSDLWindow, true);
	SDL_SetWindowFullscreenMode(mSDLWindow, &fullScreenDisplayMode);
}

void Window::SetWindowed()
{
	bool result = SDL_SetWindowBordered(mSDLWindow, true);
	SDL_SetWindowFullscreen(mSDLWindow, false);
}

void Window::SetBorderless()
{
	SDL_SetWindowBordered(mSDLWindow, false);
	SDL_SetWindowFullscreen(mSDLWindow, false);
}



void Window::InitSDL()
{
	if (!SDL_Init(SDL_INIT_CAMERA))
		std::cout << "SDL init failed!";
}

void Window::InitBgfx()
{
	SDL_PropertiesID sdlPropertiesId = SDL_GetWindowProperties(mSDLWindow);
	bgfx::PlatformData pd{};
#ifdef OS_WINDOWS
	pd.nwh = SDL_GetPointerProperty(sdlPropertiesId, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	pd.ndt = NULL;
#elif OS_LINUX
	if (!Platform_SetLinuxPlatformData(sdlPropertiesId, pd))
	{
		std::cout << "ERROR: failed to identify linux platform data\n";
		return;
	}
#elif OS_MAC
	pd.nwh = SDL_GetPointerProperty(sdlPropertiesId, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
	pd.ndt = NULL;
#endif
	pd.context = NULL;
	pd.backBuffer = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);

	bgfx::Init init;
	init.type = PLATFORM_BGFX_RENDERERTYPE;
	init.vendorId = BGFX_PCI_ID_NONE;
	init.platformData.nwh = pd.nwh;
	init.platformData.ndt = pd.ndt;
	init.resolution.width = mWidth;
	init.resolution.height = mHeight;
	init.callback = new BgfxNullCallback();
	if (!bgfx::init(init))
	{
		std::cout << "ERROR: failed to initialize bgfx\n";
		return;
	}
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, mWidth, mHeight);

}

void Window::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_Implbgfx_Init(255);
#if OS_WINDOWS
	ImGui_ImplSDL3_InitForD3D(mSDLWindow);
#elif OS_MAC
	ImGui_ImplSDL3_InitForMetal(mSDLWindow);
#elif OS_LINUX
	ImGui_ImplSDL3_InitForVulkan(mSDLWindow);
#endif
}


