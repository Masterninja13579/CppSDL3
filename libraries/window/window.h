#pragma once

#include "core.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Application
{
	class Window
	{
	public:
		typedef std::function<void(const std::vector<std::string>&)> DialogCallback;
		typedef std::vector<SDL_DialogFileFilter> Filters;

		Window(const std::string&);
		Window(const std::string&, int, int, SDL_WindowFlags);

		static void		PrintSDLFlags(const SDL_WindowFlags&);

		const std::string&	GetName();
		int					GetWidth();
		int					GetHeight();
		SDL_WindowFlags		GetSDLWindowFlags();
		SDL_WindowID		GetSDLWindowId();
		bool				IsFullScreen();
		bool				IsBorderless();
		bool				IsWindowed();
		bool				IsMinimized();
		bool				IsMaximized();
		bool				IsShowingDialog();

		void Create();
		void Destroy();
		void Refresh();
		void Resize(int, int);
		void SetName(const std::string&);
		void SetFullScreen();
		void SetFullScreen(const SDL_DisplayMode&);
		void SetWindowed();
		void SetBorderless();

		bool ShowOpenFileDialog(DialogCallback, const std::string&, Filters = {}, bool = false);
		bool ShowOpenFolderDialog(DialogCallback, const std::string&);
		bool ShowSaveFileDialog(DialogCallback, const std::string&, Filters = {});

	private:
		std::string mName;
		int mWidth;
		int mHeight;
		SDL_WindowFlags mFlags;

		SDL_Window* mSDLWindow;
		bool mIsShown;

		bool mIsDialogOpen;
		DialogCallback mDialogCallback;
		std::string mDialogPath;
		Filters mDialogFilters;

		void InitSDL();
		void InitBgfx();
		void InitImGui();

		static void SdlDialogCallback(void*, const char* const*, int);
	};

	/// <summary>
	/// SDL window flags mapped to string names.
	/// </summary>
	const std::unordered_map<Uint64, std::string> WindowFlagMap
	{
		{ SDL_WINDOW_FULLSCREEN,			"SDL_WINDOW_FULLSCREEN" },
		{ SDL_WINDOW_OPENGL,				"SDL_WINDOW_OPENGL" },
		{ SDL_WINDOW_OCCLUDED,				"SDL_WINDOW_OCCLUDED" },
		{ SDL_WINDOW_HIDDEN,				"SDL_WINDOW_HIDDEN" },
		{ SDL_WINDOW_BORDERLESS,			"SDL_WINDOW_BORDERLESS" },
		{ SDL_WINDOW_RESIZABLE,				"SDL_WINDOW_RESIZABLE" },
		{ SDL_WINDOW_MINIMIZED,				"SDL_WINDOW_MINIMIZED" },
		{ SDL_WINDOW_MAXIMIZED,				"SDL_WINDOW_MAXIMIZED" },
		{ SDL_WINDOW_MOUSE_GRABBED,			"SDL_WINDOW_MOUSE_GRABBED" },
		{ SDL_WINDOW_INPUT_FOCUS,			"SDL_WINDOW_INPUT_FOCUS" },
		{ SDL_WINDOW_EXTERNAL,				"SDL_WINDOW_EXTERNAL" },
		{ SDL_WINDOW_MODAL,					"SDL_WINDOW_MODAL" },
		{ SDL_WINDOW_HIGH_PIXEL_DENSITY,	"SDL_WINDOW_HIGH_PIXEL_DENSITY" },
		{ SDL_WINDOW_MOUSE_CAPTURE,			"SDL_WINDOW_MOUSE_CAPTURE" },
		{ SDL_WINDOW_MOUSE_RELATIVE_MODE,	"SDL_WINDOW_MOUSE_RELATIVE_MODE" },
		{ SDL_WINDOW_ALWAYS_ON_TOP,			"SDL_WINDOW_ALWAYS_ON_TOP" },
		{ SDL_WINDOW_UTILITY,				"SDL_WINDOW_UTILITY" },
		{ SDL_WINDOW_TOOLTIP,				"SDL_WINDOW_TOOLTIP" },
		{ SDL_WINDOW_POPUP_MENU,			"SDL_WINDOW_POPUP_MENU" },
		{ SDL_WINDOW_KEYBOARD_GRABBED,		"SDL_WINDOW_KEYBOARD_GRABBED" },
		{ SDL_WINDOW_FILL_DOCUMENT,			"SDL_WINDOW_FILL_DOCUMENT" },
		{ SDL_WINDOW_VULKAN,				"SDL_WINDOW_VULKAN" },
		{ SDL_WINDOW_METAL,					"SDL_WINDOW_METAL" },
		{ SDL_WINDOW_TRANSPARENT,			"SDL_WINDOW_TRANSPARENT" },
		{ SDL_WINDOW_NOT_FOCUSABLE,			"SDL_WINDOW_NOT_FOCUSABLE" }
	};

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


}