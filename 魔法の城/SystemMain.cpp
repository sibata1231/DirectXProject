#include "SystemMain.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "DirectGraphics.h"

// 外部
#include "Sound.h"
#include "Input.h"
#include "Light.h"

// ポリゴン用
#include "ModelManager.h"
#include "Mesh.h"
#include "Polygon.h"

// 各シーン
#include "sceneManager.h"
#include "sceneAnimation.h"
#include "sceneLoading.h"
#include "sceneTitle.h"
#include "sceneGame.h"

// 初期化処理
HRESULT SystemMain::Init() {
    HRESULT hr = S_OK;
    CSound::Init();               // サウンド初期化処理
    Input::GetInstance().Init();  // 入力初期化処理
    Light::GetInstance().Init();  // ライト初期化

    // ポリゴン
    hr = PolygonManager::GetInstance().Init(DirectGraphics::GetInstance().GetDevice());

    // シーン
    SceneManager::GetInstance().Active();
    SceneManager::GetInstance().Init(new SceneTitle());
    return hr;
}

// 終了処理
void SystemMain::Uninit(void) {
    SceneManager::GetInstance().Uninit();         // シーン
    SceneManager::GetInstance().Inactive();       // フェードシーン
    Synchronization::GetInstance()._sync.Abort(); // 非同期処理

    PolygonManager::GetInstance().Uninit(); // UI
    ModelManager::GetInstance().Uninit();   // モデル

    Light::GetInstance().Uninit(); // ライト
    Input::GetInstance().Uninit(); // 入力
    CSound::Fin();                 // サウンド終了
}

void SystemMain::Debug() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    std::string fps;

    ImGui::Begin("[Inspector]");
    if (ImGui::CollapsingHeader("System")) {
        if (ImGui::TreeNode("Main")) {
            ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
            ImGui::ColorEdit4("BackGroundColor", (float *)&DirectGraphics::GetInstance().m_backgroundColor);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Camera")) {
            ImGui::Checkbox("[Control]", &MainCamera::GetInstance().m_isController);
            ImGui::Text("Eye  [%.3f,%.3f,%.3f]", MainCamera::GetInstance().GetEye().x, MainCamera::GetInstance().GetEye().y, MainCamera::GetInstance().GetEye().z);
            ImGui::Text("Look [%.3f,%.3f,%.3f]", MainCamera::GetInstance().GetLook().x, MainCamera::GetInstance().GetLook().y, MainCamera::GetInstance().GetLook().z);
            ImGui::Text("Up   [%.3f,%.3f,%.3f]", MainCamera::GetInstance().GetUpVector().x, MainCamera::GetInstance().GetUpVector().y, MainCamera::GetInstance().GetUpVector().z);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Light")) {
            ImGui::DragFloat3("LightPosition",  (float *)&Light::GetInstance().m_position);
            ImGui::DragFloat3("LightDirection", (float *)&Light::GetInstance().m_direction, 0.01f);
            ImGui::ColorEdit4("LightAmbient",   (float *)&Light::GetInstance().m_ambient);
            ImGui::ColorEdit4("LightDiffuse",   (float *)&Light::GetInstance().m_diffuse);
            ImGui::ColorEdit4("LightSpecular",  (float *)&Light::GetInstance().m_specular);
            ImGui::TreePop();
        }
    }
    SceneManager::GetInstance().Debug();
    ImGui::End();
}

// 更新処理
void SystemMain::Update() {
    CSound::Update();           // サウンド更新処理
    THE_INPUT.Update();
    SceneManager::GetInstance().Update(); // シーン更新
}

// 描画処理
void SystemMain::Draw() {
    DirectGraphics::GetInstance().StartRendering();
    DirectGraphics::GetInstance().UpdateCamera();
    DirectGraphics::GetInstance().UpdateLight();

    SceneManager::GetInstance().Draw();    // シーン描画
#ifdef _DEBUG
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    DirectGraphics::GetInstance().FinishRendering();
}