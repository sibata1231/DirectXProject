#include "sceneManager.h"
#include <iostream>
#include "Object.h"

#include "sceneLoading.h"
#include "DirectGraphics.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"

SceneManager::SceneManager() {

}

void SceneManager::Active() {
    m_fadeScene.reset(new SceneLoading());
    m_fadeScene->Init();
}

void SceneManager::Inactive() {
    m_fadeScene->Uninit();
}

void SceneManager::Init(Scene* scene) {
    scene->Init();
    m_scenes.push_back(scene);
}

void SceneManager::Uninit() {
    for (auto itr = m_scenes.begin(); itr != m_scenes.end(); ++itr) {
        Scene* scene = *itr;
        scene->Uninit();
        SAFE_DELETE(scene);
    }
    m_scenes.clear();
}

void SceneManager::Update() {
    for (auto itr = m_scenes.begin(); itr != m_scenes.end(); itr++) {
        Scene* scene = *itr;
        scene->Update();
    }
}

void SceneManager::Debug() {
    for (auto itr = m_scenes.begin(); itr != m_scenes.end(); itr++) {
        Scene* scene = *itr;
        scene->Debug();
    }
}

void SceneManager::Draw() {
    for (auto itr = m_scenes.begin(); itr != m_scenes.end(); itr++) {
        Scene* scene = *itr;
        scene->Draw();
    }
    if (m_isLoadScene) {
        ChangeScene(m_loadScene,m_loadMode);
    }
}

std::future<int> SceneManager::DOFade(){
    int i = 0;
    m_isFadeTween = true;
    m_fadeScene->Update();
    m_fadeScene->Draw();
    while (m_isLoadScene) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
        m_fadeScene->Update();
        DirectGraphics::GetInstance().StartRendering();
        DirectGraphics::GetInstance().SetUpdateCamera();
        DirectGraphics::GetInstance().SetUpdateLight();
        m_fadeScene->Draw();
        DirectGraphics::GetInstance().FinishRendering();
    }

    m_isFadeTween = false;
    return i;
}

void SceneManager::ChangeScene(Scene* loadScene, LoadModes load) {
    int count = 0;
    if (!m_isLoadScene) {
        if (m_loadScene) {
            return;
        }
        m_isLoadScene = true;
        m_loadMode    = load;
        m_loadScene   = loadScene;
        return;
    }
    switch (load) {
        case LoadModes::SINGLE:
            DOFade();
            SceneManager::Uninit();
            SceneManager::Init(loadScene);
            break;
        case LoadModes::ADDITIVE:
            for (auto itr = m_scenes.begin(); itr != m_scenes.end(); itr++) {
                Scene* scene = *itr;
                if (scene->m_sceneName == loadScene->m_sceneName) {
                    SAFE_DELETE(loadScene);
                    break;
                }
                count++;
            }
            if (count == m_scenes.size()) {
                //DOFade();
                loadScene->Init();
                m_scenes.push_back(loadScene);
            }
            break;
        default:
            SAFE_DELETE(loadScene);
            break;
    }
    m_isLoadScene = false;
    m_loadMode = LoadModes::SINGLE;
    m_loadScene = nullptr;
}