
#include "window.h"
#include <iostream>

using namespace Application;

namespace
{
	struct BgfxNullCallback : public bgfx::CallbackI
	{
		virtual void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override {}
		virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override {}
		virtual void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override {}
		virtual void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override {}
		virtual void profilerEnd() override {}
		virtual uint32_t cacheReadSize(uint64_t _id) override { return 0; }
		virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override { return false; }
		virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override {}
		virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) override {}
		virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) override {}
		virtual void captureEnd() override {}
		virtual void captureFrame(const void* _data, uint32_t _size) override {}
	};

	BgfxNullCallback* GetBgfxNullCallback()
	{
		static BgfxNullCallback callback;
		return &callback;
	}
}

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

const char*		Window::GetName() { return mName; }
int				Window::GetWidth() { return mWidth; }
int				Window::GetHeight() { return mHeight; }
SDL_WindowFlags	Window::GetSDLWindowFlags() { return SDL_GetWindowFlags(mSDLWindow); }
SDL_WindowID	Window::GetSDLWindowId() { return SDL_GetWindowID(mSDLWindow); }
//SDL_Window*	Window::GetSDLWindow() { return mSDLWindow;  }


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



void Window::InitSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "ERROR: failed to initialize SDL video subsystem.\n";
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
	init.callback = GetBgfxNullCallback();
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


