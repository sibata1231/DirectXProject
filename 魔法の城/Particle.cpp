#include "Particle.h"
#include "light.h"
#include "camera.h"

void Particle::Start() {
    m_prePos            = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_lifeTime          = 0;
    m_isEnable          = false;
    m_animation.m_count = 0.0f;
    m_animation.m_size  = XMINT2(1, 1);
    m_animation.m_mode  = AnimationMode::NONE;
    m_rigidbody         = m_parent->AddComponentIf<Rigidbody>();
}

void Particle::Update() {
	if (!m_isEnable) return;
	m_lifeTime--;
	if (m_lifeTime < 0) {
		m_lifeTime = 0;
        m_isEnable = false;
        m_transform->m_scale = Vector3(0, 0, 0);
		return;
	}
	if (m_animation.m_mode != AnimationMode::NONE) {
		Animation();
	}

    XMMATRIX mtxWorld = XMMatrixIdentity();					    // ワールドマトリックスの初期化
    XMFLOAT4X4& mtxView = MainCamera::GetInstance().GetView();	// ビューマトリックスを取得
    XMStoreFloat4x4(&m_transform->m_world, mtxWorld);	        // ワールドマトリックスの初期化
    
    // ポリゴンを正面に向ける(ビルボード処理)
    m_transform->m_world._11 = mtxView._11;
    m_transform->m_world._12 = mtxView._21;
    m_transform->m_world._13 = mtxView._31;
    m_transform->m_world._21 = mtxView._12;
    m_transform->m_world._22 = mtxView._22;
    m_transform->m_world._23 = mtxView._32;
    m_transform->m_world._31 = mtxView._13;
    m_transform->m_world._32 = mtxView._23;
    m_transform->m_world._33 = mtxView._33;
    
    mtxWorld = XMLoadFloat4x4(&m_transform->m_world);
    
    // 拡縮
    XMMATRIX mtxScale = XMMatrixScaling(m_transform->m_scale.x, m_transform->m_scale.y, 1.0f);
    mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);
    
    // 回転
    XMMATRIX rotate_x = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_transform->m_rotation.x));
    XMMATRIX rotate_y = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_transform->m_rotation.y));
    XMMATRIX rotate_z = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_transform->m_rotation.z));

    // 移動
    XMMATRIX mtxTranslate = XMMatrixTranslation(m_transform->m_position.x, m_transform->m_position.y, m_transform->m_position.z);
    mtxWorld *= rotate_x * rotate_y * rotate_z * mtxTranslate;
    
    // ワールドマトリックスの設定
    XMStoreFloat4x4(&m_transform->m_world, mtxWorld);
}

void Particle::Draw() {}

void Particle::Animation() {
    if (m_animation.m_mode == AnimationMode::NONE) {
        return;
    }
	m_animation.m_count += m_animation.m_frame;
	if (m_animation.m_count > m_animation.m_size.x * m_animation.m_size.y) {
		if (m_animation.m_mode == AnimationMode::ONCE_PLAY || m_animation.m_mode == AnimationMode::ONCE_REPLAY) {
			m_animation.m_mode = AnimationMode::NONE;
		}
		m_animation.m_count = 0;
	}
}