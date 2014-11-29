#include "AddFieldWindow.h"

LRESULT CALLBACK AddFieldWindow::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#define OK_BUTTON 0
#define X_EDIT 1
#define Y_EDIT 2
#define Z_EDIT 3

	HWND ok,xedit,yedit,zedit, charge, mass, fixed;

	switch (msg)
	{
	case WM_CREATE:
	{
		xedit = CreateWindowEx(	WS_EX_CLIENTEDGE,
								"EDIT",
								"Enter x position", 
								WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
								0 + MARGIN_SIZE,
								0 + MARGIN_SIZE,
								AddFieldWindow::width - 2 * MARGIN_SIZE - DIMENSION,
								DIMENSION,
								hwnd,
								(HMENU)X_EDIT,
								GetModuleHandle(NULL),
								NULL);

		yedit = CreateWindowEx(	WS_EX_CLIENTEDGE,
								"EDIT",
								"Enter y position", 
								WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
								0 + MARGIN_SIZE,
								2*DIMENSION + MARGIN_SIZE,
								AddFieldWindow::width - 2 * MARGIN_SIZE - DIMENSION,
								DIMENSION,
								hwnd,
								(HMENU)Y_EDIT,
								GetModuleHandle(NULL),
								NULL);

		zedit = CreateWindowEx(	WS_EX_CLIENTEDGE,
								"EDIT",
								"Enter z position", 
								WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
								0 + MARGIN_SIZE,
								4 * DIMENSION + MARGIN_SIZE,
								AddFieldWindow::width - 2 * MARGIN_SIZE - DIMENSION,
								DIMENSION,
								hwnd,
								(HMENU)Z_EDIT,
								GetModuleHandle(NULL),
								NULL);

		ok = CreateWindowEx(	NULL,
								"BUTTON",
								"Add Field Sensor", 
								WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
								0 + MARGIN_SIZE,
								12 * DIMENSION + MARGIN_SIZE,
								AddFieldWindow::width - 2 * MARGIN_SIZE - DIMENSION,
								3 * DIMENSION/2,
								hwnd,
								(HMENU)OK_BUTTON,
								GetModuleHandle(NULL),
								NULL);

		break;
	}
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case OK_BUTTON:
		{
			float x, y, z;
			TCHAR buffer[1024];
			GetWindowText(GetDlgItem(hwnd,X_EDIT),buffer,1024);
			x = atof(buffer);
			GetWindowText(GetDlgItem(hwnd,Y_EDIT),buffer,1024);
			y = atof(buffer);
			GetWindowText(GetDlgItem(hwnd,Z_EDIT),buffer,1024);
			z = atof(buffer);
			printf("%.2f,%.2f,%.2f\n",x,y,z);

			AddFieldWindow::tempVector = Vector3(x,y,z + _zdefault);
			DestroyWindow(hwnd);
			break;
		}
		default:
			break;
		}
		break;
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

AddFieldWindow::AddFieldWindow(int _width, int _height, char _title[], char className[], Vector3 *_cameraPos) :
	title(_title)
{
	char clsname[10];
	cameraPos = _cameraPos;
	width = _width;
	height = _height;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= 0;
	wc.lpfnWndProc		= AddFieldWindow::StaticWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	_itoa_s(counter++,clsname,10);
	wc.lpszClassName	= clsname;
	wc.hbrBackground	= CreateSolidBrush(RGB(150,200,200));
	wc.lpszMenuName		= 0;
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	wndclassname = clsname;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error Registering Window", "Error", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, clsname, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);

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

AddFieldWindow::~AddFieldWindow(void)
{
}

int AddFieldWindow::width;
int AddFieldWindow::height;
Vector3 * AddFieldWindow::cameraPos;

Vector3 AddFieldWindow::tempVector;
float AddFieldWindow::Mass;
float AddFieldWindow::Charge;
bool AddFieldWindow::Fixed;

unsigned int AddFieldWindow::counter = 1000;