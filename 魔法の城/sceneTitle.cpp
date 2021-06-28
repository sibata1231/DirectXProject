#include "sceneManager.h"
#include "sceneTitle.h"
#include "sceneGame.h"

#include "GraphicsUtility.h"
#include "Input.h"

#include "Player.h"
#include "Polygon.h"
#include "Font.h"

SceneTitle::SceneTitle() : Scene::Scene(&SceneManager::GetInstance()) {}

void SceneTitle::Init() {
    Scene::Init();
    m_sceneName = "Title";

    // 背景作成
    /*{
        Object* object = new Object(nullptr);
        m_sky = object->AddComponent<Sky>();
        m_objectManager.Add(object);
    }*/

    // フォント画像
    {
        Object* object = new Object(nullptr);
        object->m_transform->m_name = "FontTest";
        object->AddComponent<Font>();
        m_objectManager.Add(object);
    }

    // DepthWrite画像
    {
        Object* object = new Object(nullptr);
        object->m_transform->m_name = "DepthWriteImage";
        Texture* texture = object->AddComponent<Texture>();
        texture->Load(DirectGraphics::GetInstance().GetRenderTargetTexture("DepthWrite"));
        RectTransform *transform = object->GetComponent<RectTransform>();
        transform->m_position.x = -450.0f;
        transform->m_position.y = 224.0f;
        transform->m_scale.x = 128.0f;
        transform->m_scale.y = 128.0f;
        m_objectManager.Add(object);
    
    }
    // プレイヤーの生成
    {
        Object* object = new Object(nullptr);
        object->AddComponent<Player>();
        object->m_transform->m_name = "Player";
        m_objectManager.Add(object);
    }
}

void SceneTitle::Uninit() {
    Scene::Uninit();
}

void SceneTitle::Update() {
	Scene::Update();

    if (Input::GetInstance().GetKeyTrigger(VK_RETURN) || Input::GetInstance().GetKeyTrigger(VK_2)) {
        m_sceneManager->ChangeScene(new SceneGame());
    }
}

void SceneTitle::Debug() {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "<Scene : Title >\n");
    Scene::Debug();
}

void SceneTitle::Draw() {
    Scene::Draw();
}