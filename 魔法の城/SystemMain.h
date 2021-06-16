#ifndef ___SYSTEM_MAIN_H___
#define ___SYSTEM_MAIN_H___

#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include "Singleton.h"
#include "imgui.h"
#include "Window.h"

using namespace DirectX;

class SystemMain : public Singleton<SystemMain> {
public:
    bool	m_dispDebug = true;	             //!< デバッグ表示ON/OFF
    ImVec4  m_backgroundColor = ImVec4(0.117647f, 0.254902f, 0.352941f, 1.0f);
public:
    HRESULT Init();
    void    Uninit();
    void    Update();
    void    Debug();
    void    Draw();
};

typedef Singleton<SystemMain> SystemMainSingleton;
#define THE_SYSTEM            SystemMainSingleton::GetInstance()

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x){(x)->Release();x=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete(x);x=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) {if(x){delete[](x);x=nullptr;}}
#endif

#define HRESULT_CHECK(hr,name) \
    if (FAILED(hr)) { \
        MessageBox(FindWindow(Window::ClassName, nullptr), _T(name),WINDOW_NAME, MB_OK);\
        return hr;\
    }\

#define GetAime(ret,px1,px2,py1,py2) { \
	float dx = px2 - px1;	\
	float dy = py2 - py1;	\
	float rad = atan2(dy, dx);	\
	ret = rad; \
}\

#endif