#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "Panel.h"
#include "GameData.h"

#define PATH_PLAYER ("Resources/model/Dragon.fbx") // ƒ‚ƒfƒ‹
#define SPEED       (15.0f)                   // ‘¬‚³
#define ROLL_SPEED  (2.0f)
#define PITCH_SPEED (2.0f)
#define PLAYER_MOVE (50.0f)

Player:: Player() {}
Player::~Player() {}

void Player::Start() {
    Model* model             = m_parent->AddComponentIf<Model>();
    model->Create("Player", PATH_PLAYER);
    m_transform->m_name      = "Player";
    m_transform->m_tagName   = "Player";
    m_transform->m_position  = Vector3(  0.0f, 0.0f, 0.0f);
    m_transform->m_rotation  = Vector3(-90.0f, 180.0f, 0.0f);
    m_renderer               = m_parent->GetComponent<ModelRenderer>();
    m_renderer->m_shaderName = "DepthShadow";
    m_renderer->m_shadowType = ShadowTypes::TARGET;
    // NormalModel
    // RimLight
    // Bokeh
    // Fog
}

void Player::MoveUp() {
    if (m_transform->IsPlay()) {
        return;
    }
    if (PanelManager::GetInstance().IsCheck(Vector3(m_transform->m_position.x, m_transform->m_position.y, m_transform->m_position.z - PLAYER_MOVE))) {
        m_transform->DORotate(Vector3(-90.0f, 180.0f, 0.0f), 0.08f);
        m_transform->DOMoveZ(-PLAYER_MOVE, 0.1f).SetRelative();
        PanelManager::GetInstance().Delete(m_transform->m_position);
    }
}
void Player::MoveDown() {
    if (m_transform->IsPlay()) {
        return;
    }
    if (PanelManager::GetInstance().IsCheck(Vector3(m_transform->m_position.x, m_transform->m_position.y, m_transform->m_position.z + PLAYER_MOVE))) {
        m_transform->DORotate(Vector3(-90.0f, 360.0f, 0.0f), 0.08f);
        m_transform->DOMoveZ(PLAYER_MOVE, 0.1f).SetRelative();
        PanelManager::GetInstance().Delete(m_transform->m_position);
    }

}
void Player::MoveLeft() {
    if (m_transform->IsPlay()) {
        return;
    }
    if (PanelManager::GetInstance().IsCheck(Vector3(m_transform->m_position.x + PLAYER_MOVE, m_transform->m_position.y, m_transform->m_position.z))) {
        m_transform->DORotate(Vector3(-90.0f, 90.0f, 0.0f), 0.08f);
        m_transform->DOMoveX(PLAYER_MOVE, 0.1f).SetRelative();
        PanelManager::GetInstance().Delete(m_transform->m_position);
    }
}
void Player::MoveRight() {
    if (m_transform->IsPlay()) {
        return;
    }
    if (PanelManager::GetInstance().IsCheck(Vector3(m_transform->m_position.x - PLAYER_MOVE, m_transform->m_position.y, m_transform->m_position.z))) {
        m_transform->DORotate(Vector3(-90.0f, 270.0f, 0.0f), 0.08f);
        m_transform->DOMoveX(-PLAYER_MOVE, 0.1f).SetRelative();
        PanelManager::GetInstance().Delete(m_transform->m_position);
    }
}

void Player::Update() {

}

void Player::Draw() {

}
void PlayerController::Register(Player* player) {
    m_player = player;
}

void PlayerController::Input() {
    if (THE_INPUT.GetKeyTrigger(VK_W)) {
        m_player->MoveUp();
    }
    if (THE_INPUT.GetKeyTrigger(VK_S)) {
        m_player->MoveDown();
    }
    if (THE_INPUT.GetKeyTrigger(VK_A)) {
        m_player->MoveLeft();
    }
    if (THE_INPUT.GetKeyTrigger(VK_D)) {
        m_player->MoveRight();
    }
}