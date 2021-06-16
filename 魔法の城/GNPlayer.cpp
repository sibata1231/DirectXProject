#include "GNPlayer.h"
#include "sceneNetwork.h"

#ifdef _CLIENT
#include "Client.h"    
#else
#include "Server.h"
#endif // _CLIENT

// マクロ定義
#define PATH_MODEL_PLAYER ("Resources/model/Dragon.fbx") // モデル

// コンストラクタ
GNPlayer::GNPlayer() {

}

// デストラクタ
GNPlayer::~GNPlayer() {

}

void GNPlayer::Start() {
    Model* model = m_parent->AddComponentIf<Model>();
    model->Create("Player", PATH_MODEL_PLAYER);
    m_transform->m_name    = "Player";
    m_transform->m_tagName = "Player";
    m_transform->m_position = Vector3(0.0f, -150.0f, 400.0f);
    m_transform->m_rotation = Vector3(-90.0f, -180.0f, 0.0f);
}

void GNPlayer::Update() {
#ifdef _CLIENT
    m_transform->m_position = Vector3(
        Client::GetInstance().m_myClient.posX,
        Client::GetInstance().m_myClient.posY,
        Client::GetInstance().m_myClient.posZ);
#else

#endif // _CLIENT
}

void GNPlayer::Draw() {

}
