#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#include "Window.h"

const char *Window::ClassName  = "DX11PracticeWindow";
HINSTANCE   Window::Instacne = nullptr;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window::WindowProc(HWND window_handle, UINT message_id, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(window_handle, message_id, wparam, lparam)) {
        return true;
    }

	switch (message_id) {
	    case WM_CLOSE:
	    	PostQuitMessage(0);
	    	break;
	    default:
	    	return DefWindowProc(window_handle, message_id, wparam, lparam);
	    	break;
	}

	return 0;
}

bool Window::Create()
{
	if (EntryWindowClass() == false)
	{
		return false;
	}

	// ウィンドウ作成
	HWND window_handle = CreateWindowA(
		Window::ClassName,
		m_Title,
		(WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) | WS_VISIBLE,
		CW_USEDEFAULT,
		0,
		m_Width,
		m_Height,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);

	if (window_handle == NULL)
	{
		return false;
	}

	ResizeWindow(window_handle);

	return true;
}

bool Window::EntryWindowClass() {
	WNDCLASSEXA window_class = {
		sizeof(WNDCLASSEX),				// 構造体のサイズ
		CS_HREDRAW | CS_VREDRAW,		// クラスのスタイル
		WindowProc,						// ウィンドウプロシージャ
		0,								// 補助メモリ
		0,								// 補助メモリ
		GetModuleHandle(NULL),			// このプログラムのインスタンスハンドル
		LoadIcon(NULL, (LPSTR)MAKEINTRESOURCE(IDI_APPLICATION)),// アイコン画像
		LoadCursor(NULL, IDC_ARROW),	// カーソル画像
		NULL,							// 背景ブラシ(背景色)
		NULL,							// メニュー名
		Window::ClassName,				// クラス名									
		NULL							// 小さいアイコン
	};

	// 構造体の登録
	if (RegisterClassExA(&window_class) == 0)
	{
		return false;
	}

	return true;
}

void Window::ResizeWindow(HWND window_handle)
{
	RECT window_rect;
	RECT client_rect;

	GetWindowRect(window_handle, &window_rect);
	GetClientRect(window_handle, &client_rect);

	int frame_size_x = (window_rect.right - window_rect.left) - (client_rect.right - client_rect.left);
	int frame_size_y = (window_rect.bottom - window_rect.top) - (client_rect.bottom - client_rect.top);

	SetWindowPos(window_handle, NULL, CW_USEDEFAULT, 0, frame_size_x + m_Width, frame_size_y + m_Height, SWP_NOMOVE);

	ShowWindow(window_handle, SW_SHOW);
	UpdateWindow(window_handle);
}
