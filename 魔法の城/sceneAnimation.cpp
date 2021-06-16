#include "sceneAnimation.h"
#include "GraphicsUtility.h"
#include "Sky.h"
#include "GMPlayer.h"
#include "Polygon.h"

SceneAnimation::SceneAnimation(SceneController* sceneManager) : Scene::Scene(sceneManager) {}

void SceneAnimation::Init() {
	Scene::Init();

    // 背景作成
    Object* sky = new Object(nullptr);
    sky->AddComponent<Sky>();

    // プレイヤー作成
    Object* player = new Object(nullptr);
    player->AddComponent<GM31::Player>();

    // 2D画像の作成
    //Object* image = new Object();
    //image->m_transform->m_name = std::to_string(0) + std::string("_image");
    //Texture* texture = image->AddComponent<Texture>();
    //texture->Load("Resources/texture/_sky.png");
    //RectTransform *transform = image->GetComponent<RectTransform>();
    //transform->m_position.x = -462.0f;
    //transform->m_position.y =  240.0f;
    //transform->m_scale.x    =  100.0f;
    //transform->m_scale.y    =  100.0f;
}

void SceneAnimation::Uninit() {
	Scene::Uninit();
}

void SceneAnimation::Update() {
	Scene::Update();
}

void SceneAnimation::Debug() {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "<Scene : AnimationScene >");
}

void SceneAnimation::Draw() {
    Scene::Draw();
}