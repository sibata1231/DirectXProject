#include "sceneTitle.h"
#include "GraphicsUtility.h"
#include "Sky.h"
#include "Player.h"
#include "sceneGame.h"
#include "Input.h"
#include "Terrain.h"
#include "RightWall.h"
#include "LeftWall.h"
#include "Polygon.h"
#include "sceneManager.h"
#include "Font.h"

SceneTitle::SceneTitle() : Scene::Scene(&SceneManager::GetInstance()) {}

void SceneTitle::Init() {
    Scene::Init();
    m_sceneName = "Title";

    m_stage = Stages::RIVER;
    m_preStages = Stages::RIVER;
    m_stageCount = 0;

    // 背景作成
    /*{
        Object* object = new Object(nullptr);
        m_sky = object->AddComponent<Sky>();
        m_objectManager.Add(object);
    }*/

    // テスト画像
    {
        Object* object = new Object(nullptr);
        object->m_transform->m_name = "DepthWriteImage";
        Texture* texture = object->AddComponent<Texture>();
        //texture->Load("Resources/texture/_sky.png");
        texture->Load(DirectGraphics::GetInstance().GetRenderTargetTexture("DepthWrite"));
        RectTransform *transform = object->GetComponent<RectTransform>();
        transform->m_position.x = -450.0f;
        transform->m_position.y = 224.0f;
        transform->m_scale.x = 128.0f;
        transform->m_scale.y = 128.0f;
        m_objectManager.Add(object);
    }
    // テスト画像
    {
        Object* object = new Object(nullptr);
        object->AddComponent<Font>();
        m_objectManager.Add(object);
    }
    // テスト画像
    //{
    //    Object* object = new Object(nullptr);
    //    object->m_transform->m_name = "DepthWriteImage";
    //    Texture* texture = object->AddComponent<Texture>();
    //    //texture->Load("Resources/texture/_sky.png");
    //    texture->Load(DirectGraphics::GetInstance().GetRenderTargetTexture("DepthWrite"));
    //    RectTransform *transform = object->GetComponent<RectTransform>();
    //    transform->m_position.x = -450.0f + 128.0f;
    //    transform->m_position.y = 224.0f;
    //    transform->m_scale.x = 128.0f;
    //    transform->m_scale.y = 128.0f;
    //    m_objectManager.Add(object);
    //
    //}
    // プレイヤーの生成
    {
        Object* object = new Object(nullptr);
        object->AddComponent<Player>();
        m_objectManager.Add(object);
    }
    //// プレイヤーの生成
    //{
    //    Object* object = new Object(nullptr);
    //    object->AddComponent<Player>();
    //    object->m_transform->m_position = Vector3(30.0f, 0.0f, 0.0f);
    //    object->m_transform->m_name = "PlayerRight";
    //    m_objectManager.Add(object);
    //}
    //// プレイヤーの生成
    //{
    //    Object* object = new Object(nullptr);
    //    object->AddComponent<Player>();
    //    object->m_transform->m_position = Vector3(-30.0f, 0.0f, 0.0f);
    //    object->m_transform->m_name     = "PlayerLeft";
    //    m_objectManager.Add(object);
    //}
    // 地形作成
    /*{
        // テクスチャ
        static std::string baseTerrain[] = {
            "Resources/texture/Water Light Blue.png",
            "Resources/texture/lava.png",
            "Resources/texture/cloud.jpg",
        };
        static std::string normalTerrain[] = {
            "Resources/texture/Water Deep BlueNormalMap.png",
            "Resources/texture/lavaNormalMap.png",
            "Resources/texture/cloudNormalMap.png",
        };

        // Terrain作成
        for (int i = 0; i < (int)Stages::SNOW + 1; i++) {
            Object* object = new Object(nullptr);
            object->m_transform->m_name = "Terrain : Stages" + std::to_string(i);
            object->m_isActive = false;
            m_terrainList.push_back(object->AddComponent<Terrain>());
            m_terrainList[i]->Create(baseTerrain[i].c_str());
            m_terrainList[i]->CreateBumpMapTexture(normalTerrain[i].c_str());
            m_objectManager.Add(object);
        }
    }
    m_terrainList[(int)m_stage]->m_transform->m_object->m_isActive = true;
    */

    // 背景テクスチャ作成
    /*{
        Object* obejct = new Object(nullptr);
        obejct->m_transform->m_name = "TitleBackGround";

        Texture* texture = obejct->AddComponent<Texture>();
        texture->Load("Resources/texture/960_001.png");

        RectTransform *backtransform = obejct->GetComponent<RectTransform>();
        backtransform->m_position.x = 0.0f;
        backtransform->m_position.y = 200.0f;
        backtransform->m_scale.x    = 500.0f;
        backtransform->m_scale.y    = 200.0f;
        backtransform->m_color      = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        //backtransform->DOMoveY(-200.0f, 1.0f);
        m_objectManager.Add(obejct);
    }*/

    // 2D画像の作成
    /*{
        for (int i = 0; i < 2; i++) {
            Object* object = new Object(nullptr);
            object->m_transform->m_name = std::to_string(i) + std::string("_image");

            Texture* texture = object->AddComponent<Texture>();
            texture->Load("Resources/texture/magicCastle.png");

            RectTransform *transform = object->GetComponent<RectTransform>();
            transform->m_position.x  = 20.0f - i * 2;
            transform->m_position.y  = 175.0f - i * 2;
            transform->m_scale.x     = 800.0f;
            transform->m_scale.y     = 600.0f;
            transform->m_color       = XMFLOAT4(0.0f, 1 - i * 0.5f, 1 - i * 0.5f, 1 - i * 0.5f);
            //transform->DOMoveY(-200.0f, 3.0f);
            m_objectManager.Add(object);
        }
    }*/
    // 雪エフェクト作成
    /*{
        Object* object = new Object(nullptr);
        m_snow = object->AddComponent<FxSnow>();
        object->m_isActive = false;
        m_objectManager.Add(object);
    }*/

    // 煙エフェクト作成
    /*{
        Object* object = new Object(nullptr);
        m_smoke = object->AddComponent<FxSmoke>();
        object->m_isActive = false;
        m_objectManager.Add(object);
    }*/
}

void SceneTitle::Uninit() {
    Scene::Uninit();
}

void SceneTitle::Update() {
	Scene::Update();

    if (Input::GetInstance().GetKeyTrigger(VK_RETURN) || Input::GetInstance().GetKeyTrigger(VK_2)) {
        m_sceneManager->ChangeScene(new SceneGame());
    }

    /*
    m_stageCount++;
    if (m_stageCount >= 600) {
        if (m_stage >= Stages::SNOW) {
            m_stage = Stages::RIVER;
        } else {
            m_stage = (Stages)((int)m_stage + 1);
        }    
        m_stageCount = 0;
    }
    */

    if (m_preStages != m_stage) {
        if (m_preStages == Stages::RIVER) {
        }
        if (m_preStages == Stages::MAGUMA) {
            m_smoke->m_parent->m_isActive = false;
            m_sky->m_transform->m_object->m_isActive = true;
        }
        if (m_preStages == Stages::SNOW) {
            m_snow->m_parent->m_isActive = false;
        }
        if (m_stage == Stages::RIVER) {
        }
        if (m_stage == Stages::MAGUMA) {
            m_smoke->m_parent->m_isActive = true;
            m_sky->m_transform->m_object->m_isActive = false;
        }
        if (m_stage == Stages::SNOW) {
            m_snow->m_parent->m_isActive = true;
        }
        m_terrainList[(int)m_preStages]->m_transform->m_object->m_isActive = false;
        m_preStages = m_stage;
        m_terrainList[(int)m_preStages]->m_transform->m_object->m_isActive = true;
    }
}

void SceneTitle::Debug() {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "<Scene : Title >\n");
    Scene::Debug();
    //ImGui::Text("Count : [%d]   Stage : [%d]", m_stageCount, m_stage);
    //const char* stageNames[] = {
    //    "LIVER",
    //    "MAGUMA",
    //    "SNOW",
    //};
    //ImGui::ListBox(stageNames[(int)m_stage], (int*)&m_stage, stageNames, _countof(stageNames), 2);
}

void SceneTitle::Draw() {
    Scene::Draw();
}