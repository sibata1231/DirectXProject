#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include "Singleton.h"
#include "Window.h"
#include "DirectGraphics.h"
#include "SystemMain.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//-------- ���C�u�����̃����N
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

// ���C���֐�
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
    UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

    DWORD dwExecLastTime;
    DWORD dwFPSLastTime;
    DWORD dwCurrentTime;
    DWORD dwFrameCount;

    Window window("RUN", 1024, 576);
    Window::Instacne = hInstance;

    // �E�B���h�E���쐬
    if (window.Create() == false) {
        return 0;
    }
    if (DirectGraphics::GetInstance().Init() == false) {
        return 0;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(FindWindow(Window::ClassName, nullptr));

    ID3D11Device*        device  = DirectGraphics::GetInstance().GetDevice();
    ID3D11DeviceContext* context = DirectGraphics::GetInstance().GetContext();
    ImGui_ImplDX11_Init(device, context);

#ifdef _DEBUG
    // �R���\�[�����쐬����
    AllocConsole();
    // �W�����o�͂Ɋ��蓖�Ă�
    FILE* fp = NULL;
    // ���݂̃R�[�h
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
#endif
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    // COM������
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        MessageBoxW(NULL, L"COM�̏������Ɏ��s���܂����B", L"error", MB_OK);
        return -1;
    }
    SystemMain::GetInstance().Init();
    // �t���[���J�E���g������
    timeBeginPeriod(1);				// ����\��ݒ�
    dwExecLastTime = dwFPSLastTime = timeGetTime();
    dwCurrentTime = dwFrameCount = 0;
    for (;;) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
                break;
            }
            else {
                // ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            dwCurrentTime = timeGetTime();
            if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5�b���ƂɎ��s
                dwFPSLastTime = dwCurrentTime;
                dwFrameCount = 0;
            }
            if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)) {
                dwExecLastTime = dwCurrentTime;
            }
            // �X�V����
            SystemMain::GetInstance().Update();
#ifdef _DEBUG
            // �`�揈��
            SystemMain::GetInstance().Debug();
#endif
            SystemMain::GetInstance().Draw();
            dwFrameCount++;
        }
    }
    SystemMain::GetInstance().Uninit();
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // �^�C�}�ݒ�����ɖ߂�
    timeEndPeriod(1);
    DirectGraphics::GetInstance().Release();

#ifdef _DEBUG
    FreeConsole();
#endif
    return (int)msg.wParam;
}