#pragma once

#include "core.h"

namespace Application
{
	class Window
	{
	public:
		Window(const char*);
		Window(const char*, int, int, SDL_WindowFlags);

		const char*		GetName();
		int				GetWidth();
		int				GetHeight();
		SDL_WindowFlags	GetSDLWindowFlags();
		SDL_WindowID	GetSDLWindowId();
		//SDL_Window*	GetSDLWindow();

		void Create();
		void Destroy();
		void Refresh();
		void Resize(int, int);

	private:
		const char* mName;
		int mWidth;
		int mHeight;
		SDL_WindowFlags mFlags;

		SDL_Window* mSDLWindow;
		bool mIsShown;

		void InitBgfx();
		void InitImGui();
	};


}