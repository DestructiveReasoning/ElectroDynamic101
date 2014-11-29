#pragma once

#include <Windows.h>
#include <stdio.h>
#include "ChargedParticle.h"

#define MARGIN_SIZE 16
#define DIMENSION 24

class AddParticleWindow
{
public:
	AddParticleWindow(int width, int height, char title[], char className[], Vector3 *cameraPos);
	~AddParticleWindow(void);

	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static Vector3 tempVector;
	static float Charge;
	static float Mass;
	static bool Fixed;

protected:
	WNDCLASSEX wc;
	MSG msg;
	HWND hwnd;
	char *title;
	char *wndclassname;
	static int width;
	static int height;

	static Vector3 *cameraPos;

	static unsigned int counter;

	inline static AddParticleWindow *GetWindowPointer(HWND hwnd)
	{
		AddParticleWindow *wnd;
		wnd = (AddParticleWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
		if(!wnd) printf("Cannot get window pointer\n");
		return wnd;
	}
};

