#include "scene.h"

Scene::Scene(SceneController* sceneManager) {
	m_sceneManager = sceneManager;
    m_sceneName    = "BaseScene";
    m_isActive     = true;
}

Scene::~Scene() {}

void Scene::Init() {
    m_objectManager.Init();
}

void Scene::Uninit() {
    m_objectManager.Release();
}

void Scene::Debug() {
    ImGui::Checkbox((std::string("Active : ") + m_sceneName).c_str(), &m_isActive);
    m_objectManager.Debug();
}

void Scene::Update() {
    if (!m_isActive) {
        return;
    }
    m_objectManager.Update();
}

void Scene::Draw() {
    if (!m_isActive) {
        return;
    }
    m_objectManager.Draw();
}