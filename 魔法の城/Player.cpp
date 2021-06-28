#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "Panel.h"
#include "GameData.h"

#define PATH_PLAYER ("Resources/model/Dragon.fbx") // モデル
#define OBJECT_NAME ("Player") // オブジェクト名称

Player:: Player() {}
Player::~Player() {}

void Player::Start() {
    // Model
    Model* model = m_parent->AddComponentIf<Model>();
    model->Create(OBJECT_NAME, PATH_PLAYER);

    // Transform
    m_transform->m_name      = OBJECT_NAME;
    m_transform->m_tagName   = OBJECT_NAME;
    m_transform->m_position  = Vector3(  0.0f, 0.0f, 0.0f);
    m_transform->m_rotation  = Vector3(-90.0f, 180.0f, 0.0f);

    // Renderer
    ModelRenderer* m_renderer = m_parent->GetComponent<ModelRenderer>();
    m_renderer->m_shaderName  = "DepthShadow";
    m_renderer->m_shadowType  = ShadowTypes::TARGET;
    // NormalModel
    // RimLight
    // Bokeh
    // Fog
}

void Player::Update() {

}

void Player::Draw() {

}