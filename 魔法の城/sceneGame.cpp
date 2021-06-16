#include "sceneGame.h"
#include "sceneTitle.h"
#include "sceneManager.h"
#include "Input.h"

#include "Player.h"
#include "Polygon.h"
#include "Terrain.h"
#include "Panel.h"
#include "GameData.h"

SceneGame::SceneGame() : Scene::Scene(&SceneManager::GetInstance()) {}

void SceneGame::Init() {
	Scene::Init();
    m_sceneName = "Game";

    // ゲームデータ作成
    StageInfo stage;
    stage.m_id = 0;
    stage.m_isClear = false;

    // プレイヤー作成
    {
        Object* object = new Object(nullptr);
        PlayerController::GetInstance().Register(object->AddComponent<Player>());
        stage.m_startPoint = object->m_transform->m_position;
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
    // パネル作成
    {
        PanelManager::GetInstance().Init(&m_objectManager);
        PanelManager::GetInstance().CreateStage(0);
        stage.m_size = PanelManager::GetInstance().StageSize();
    }

    GameData::GetInstance().m_stages.push_back(stage);
}

void SceneGame::Uninit() {
	Scene::Uninit();
}

void SceneGame::Update() {
	Scene::Update();

    // プレイヤー入力処理更新
    PlayerController::GetInstance().Input();

    // シーン入力更新
    if (Input::GetInstance().GetKeyTrigger(VK_1)) {
        m_sceneManager->ChangeScene(new SceneTitle());
    } else if(Input::GetInstance().GetKeyTrigger(VK_R)){
        GameData::GetInstance().Reset();
    }

}

void SceneGame::Debug() {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "<Scene : Game >");
    Scene::Debug();
    ImGui::Text("Clear Flag %d", GameData::GetInstance().m_stages[0].m_isClear);
}

void SceneGame::Draw() {
	Scene::Draw();
}