#include "GMPlayer.h"
#include "Input.h"
#include "Light.h"

#define	MAX_MODEL_PLAYER	(6)								// 読み込むモデル数
#define	PLAYER_RADIUS		(50.0f)							// 半径
#define	VALUE_MOVE_PLAYER	(0.155f)						// 移動速度
#define	RATE_MOVE_PLAYER	(0.025f)						// 移動慣性係数
#define	VALUE_ROTATE_PLAYER	XMConvertToRadians(4.5f)		// 回転速度
#define	RATE_ROTATE_PLAYER	(0.10f)							// 回転慣性係数
#define	VALUE_MOVE_BULLET	(7.5f)							// 弾の移動速度
#define	PLAYER_LIFE		    (8)                     		// ライフ

#define ANIM_IDLE	0
#define ANIM_WALK	1
#define ANIM_RUN	2
#define ANIM_JUMP	3
#define ANIM_SLASH	4
#define ANIM_KICK	5

static LPCSTR g_pszModelPlayer[MAX_MODEL_PLAYER] = {
    "Resources/model/Dragon_run01.fbx", // こいつだけ行ける

    "Resources/model/GM31/Sword And Shield Idle.fbx",
    "Resources/model/GM31/Sword And Shield Run.fbx",
    "Resources/model/GM31/Sword And Shield Jump.fbx",
    "Resources/model/GM31/Sword And Shield Slash.fbx",
    "Resources/model/GM31/Sword And Shield Kick.fbx",
    //"Resources/model/GM31/Sword And Shield Walk.fbx",
};

#define SPEED       (0.01f)                   // 速さ
#define ROLL_SPEED  (2.0f)
#define PITCH_SPEED (2.0f)

GM31::Player:: Player() {}
GM31::Player::~Player() {}

void GM31::Player::Start() {
    AnimationModel* model = m_parent->AddComponent<AnimationModel>();
    model->Create("GMPlayer" + std::to_string(0), g_pszModelPlayer[0]);
    model->m_isEnable = false;
    m_modelList.push_back(model);
    m_transform->m_position = Vector3(0.0f,   0.0f,  0.0f);
    m_transform->m_rotation = Vector3(0.0f, 180.0f, 0.0f);
    m_transform->m_scale    = Vector3(0.5f,   0.5f,  0.5f);
    m_transform->m_name     = "GMPlayer";
    m_transform->m_tagName  = "GMPlayer";

    //m_move                  = Vector3(0.0f, 0.0f, 0.0f);
    //m_rotDest               = Vector3(0.0f, 0.0f, 0.0f);
    //m_radius                = PLAYER_RADIUS;
    //m_life                  = PLAYER_LIFE;
    m_animStack             = ANIM_IDLE;
    m_animFrame             = 0.0f;
    m_modelList[m_animStack]->m_isEnable = true;
}

void GM31::Player::Update() {}

void GM31::Player::Draw() {}