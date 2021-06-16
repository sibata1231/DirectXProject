#include "sceneLoading.h"
#include "polygon.h"

#include "sceneManager.h"
#include "Player.h"

SceneLoading::SceneLoading() : Scene::Scene(&SceneManager::GetInstance()) {}

void SceneLoading::Init() {
    Scene::Init();
    m_sceneName = "Loading";

    // 背景テクスチャ作成
    {
        Object* obejct = new Object(nullptr);
        obejct->m_transform->m_name = "TitleBackGround";

        Texture* texture = obejct->AddComponent<Texture>();
        texture->Load("Resources/texture/960_001.png");

        RectTransform *backtransform = obejct->GetComponent<RectTransform>();
        backtransform->m_position.x = 0.0f;
        backtransform->m_position.y = 0.0f;
        backtransform->m_scale.x    = 1920.0f;
        backtransform->m_scale.y    = 1080.0f;
        backtransform->m_color      = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        //backtransform->DOMoveY(-200.0f, 1.0f);
        m_objectManager.Add(obejct);
    }

    // プレイヤーの生成
    {
        Object* object = new Object(nullptr);
        m_playerTransform = object->m_transform;
        object->AddComponent<Player>();
        m_objectManager.Add(object);
    }
}

void SceneLoading::Uninit() {
    Scene::Uninit();
}

void SceneLoading::Update() {
	Scene::Update();
    static float count = 0;
    count++;
    m_playerTransform->m_position.y = sinf(count) * 100.0f;

}

void SceneLoading::Debug() {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "<Scene : Loading >\n");
    Scene::Debug();
}

void SceneLoading::Draw() {
    Scene::Draw();
}