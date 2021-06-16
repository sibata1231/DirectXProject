#include "GMPlayer.h"
#include "Input.h"
#include "Light.h"

#define	MAX_MODEL_PLAYER	(6)								// “Ç‚Ýž‚Þƒ‚ƒfƒ‹”
#define	PLAYER_RADIUS		(50.0f)							// ”¼Œa
#define	VALUE_MOVE_PLAYER	(0.155f)						// ˆÚ“®‘¬“x
#define	RATE_MOVE_PLAYER	(0.025f)						// ˆÚ“®Šµ«ŒW”
#define	VALUE_ROTATE_PLAYER	XMConvertToRadians(4.5f)		// ‰ñ“]‘¬“x
#define	RATE_ROTATE_PLAYER	(0.10f)							// ‰ñ“]Šµ«ŒW”
#define	VALUE_MOVE_BULLET	(7.5f)							// ’e‚ÌˆÚ“®‘¬“x
#define	PLAYER_LIFE		    (8)                     		// ƒ‰ƒCƒt

#define ANIM_IDLE	0
#define ANIM_WALK	1
#define ANIM_RUN	2
#define ANIM_JUMP	3
#define ANIM_SLASH	4
#define ANIM_KICK	5

static LPCSTR g_pszModelPlayer[MAX_MODEL_PLAYER] = {
    "Resources/model/Dragon_run01.fbx", // ‚±‚¢‚Â‚¾‚¯s‚¯‚é

    "Resources/model/GM31/Sword And Shield Idle.fbx",
    "Resources/model/GM31/Sword And Shield Run.fbx",
    "Resources/model/GM31/Sword And Shield Jump.fbx",
    "Resources/model/GM31/Sword And Shield Slash.fbx",
    "Resources/model/GM31/Sword And Shield Kick.fbx",
    //"Resources/model/GM31/Sword And Shield Walk.fbx",
};

#define SPEED       (0.01f)                   // ‘¬‚³
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