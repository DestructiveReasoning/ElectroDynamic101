#include "Wnd32.h"

LRESULT CALLBACK Wnd32::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Wnd32 *wnd;

	if(msg == WM_CREATE)
	{
		SetWindowLong(hwnd, GWL_USERDATA, (long)((LPCREATESTRUCT(lParam))->lpCreateParams));	
	}

	wnd = GetWindowPointer(hwnd);

	if(wnd) return wnd->WndProc(hwnd,msg,wParam,lParam);
	else 
	{
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
}

LRESULT CALLBACK Wnd32::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#define OK_BUTTON 0
#define X_EDIT 1
#define Y_EDIT 2
#define Z_EDIT 3

	HWND ok,xedit,yedit,zedit;

	switch (msg)
	{
	case WM_CREATE:
	{
		xedit = CreateWindowEx(	NULL,
								"EDIT",
								"Enter x position", 
								WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
								0 + MARGIN_SIZE,
								0 + MARGIN_SIZE,
								300 - MARGIN_SIZE,
								32,
								hwnd,
								(HMENU)X_EDIT,
								GetModuleHandle(NULL),
								NULL);

		break;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 1;
}

Wnd32::Wnd32(int _width, int _height, char _title[], char className[]) :
	title(_title)
{
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= 0;
	wc.lpfnWndProc		= Wnd32::StaticWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wc.hbrBackground	= CreateSolidBrush(RGB(0,16,20));
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= className;
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	wndclassname = className;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error Registering Window", "Error", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, className, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Error", "Error", MB_ICONEXCLAMATION | MB_OK);
		return;
	}


	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, hwnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

Wnd32::~Wnd32(void)
{
}

int Wnd32::width;
int Wnd32::height;