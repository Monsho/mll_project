#include "mll/mll_defines.h"
#include "mll/mll_interfaces.h"

#include <windows.h>
#include <wingdi.h>

// Window Proc
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Handle destroy/shutdown messages.
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND InitializeWindow(HINSTANCE hInstance, int nCmdShow, int screenWidth, int screenHeight)
{
	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = "WindowClass1";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	HWND hWnd = CreateWindowEx(NULL,
		"WindowClass1",
		"D3D12Sample",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,		// We have no parent window, NULL.
		NULL,		// We aren't using menus, NULL.
		hInstance,
		NULL);		// We aren't using multiple windows, NULL.

	ShowWindow(hWnd, nCmdShow);

	return hWnd;
}

int main()
{
	HINSTANCE h_inst = ::GetModuleHandle(NULL);

	mll::DeviceDesc desc{};
	desc.SetEnableDebugLayer(true);
	auto device_ = mll::IDevice::CreateGraphicsDevice(desc);

	{
		mll::ObjPtr<mll::ICommandList> cmdlist_;
		mll::CommandListDesc cmd_desc;
		device_->CreateCommandList(cmd_desc, cmdlist_);
	}

	return 0;
}

//	EOF
