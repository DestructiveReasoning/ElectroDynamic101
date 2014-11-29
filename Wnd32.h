#pragma once

#include <Windows.h>
#include <stdio.h>

#define MARGIN_SIZE 16

class Wnd32
{
public:
	Wnd32(int width, int height, char title[], char className[]);
	~Wnd32(void);

	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WNDCLASSEX wc;
	MSG msg;
	HWND hwnd;
	char *title;
	char *wndclassname;
	static int width;
	static int height;

	inline static Wnd32 *GetWindowPointer(HWND hwnd)
	{
		Wnd32 *wnd;
		wnd = (Wnd32 *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
		if(!wnd) printf("Cannot get window pointer\n");
		return wnd;
	}
};

