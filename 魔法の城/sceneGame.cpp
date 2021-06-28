#include "sceneGame.h"
#include "sceneTitle.h"
#include "sceneManager.h"
#include "Input.h"

#include "Player.h"
#include "Polygon.h"
#include "Terrain.h"

SceneGame::SceneGame() : Scene::Scene(&SceneManager::GetInstance()) {}

void SceneGame::Init() {
	Scene::Init();
    m_sceneName = "Game";

    // プレイヤー作成
    {
        Object* object = new Object(nullptr);
        m_objectManager.Add(object);

    }
    // 地形作成
    {
        Object* object = new Object(nullptr);
        object->m_transform->m_name = "Floor";
        Terrain* terrain = object->AddComponent<Terrain>();
        terrain->Create("Resources/texture/cloud.jpg");
        terrain->CreateBumpMapTexture("Resources/texture/cloudNormalMap.png");
        m_objectManager.Add(object);
    }
}

void SceneGame::Uninit() {
	Scene::Uninit();
}

void SceneGame::Update() {
	Scene::Update();

    // シーン入力更新
    if (Input::GetInstance().GetKeyTrigger(VK_1)) {
        m_sceneManager->ChangeScene(new SceneTitle());
    }

}

void SceneGame::Debug() {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "<Scene : Game >");
    Scene::Debug();
}

void SceneGame::Draw() {
	Scene::Draw();
}