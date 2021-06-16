#include "Emitter.h"
#include "Texture.h"
#include "DirectGraphics.h"

#define TEXTURE_NAME ("Resources/texture/Default.png")

Emitter::Emitter(){}

void Emitter::Start() {
    m_texture = nullptr;
    CreateParticle(TEXTURE_NAME);

    m_speed = Vector3(0.0f, 0.0f, 0.0f);
    m_isActive = true;
    m_isGravity = true;
    m_isLooping = true;
    m_isPrewarm = true;
    m_delayTime = 0.0f;
    m_durationTime = 0.0f;
    m_lifeTime = 0.0f;
    m_emitterCountTime = 0.0f;
    m_emitterGenerate = 0;
    m_blendMode = BlendStates::ALPHABLEND;
    m_animationSize = XMINT2(0, 0);

    m_material.SetAmbient(M_AMBIENT.x, M_AMBIENT.y, M_AMBIENT.z, M_AMBIENT.w);
    m_material.SetDiffuse(M_DIFFUSE.x, M_DIFFUSE.y, M_DIFFUSE.z, M_DIFFUSE.w);
    m_material.SetSpecular(M_SPECULAR.x, M_SPECULAR.y, M_SPECULAR.z, M_SPECULAR.w);
    m_material.SetEmissive(M_EMISSIVE.x, M_EMISSIVE.y, M_EMISSIVE.z, M_EMISSIVE.w);

    // メッシュ初期化
    m_particleMesh = m_parent->AddComponentIf<ParticleMesh>();
    m_particleMesh->Create(TEXTURE_NAME, PrimitiveTypes::TRIANGLESTRIP);
}

Emitter::~Emitter() {
    m_particleList.clear();
}

void Emitter::Update()  {
	if (!m_isLooping) return;		// ループフラグOFFならSKIP
	//if (m_isPrewarm) return;		// 遅延実行フラグONならSKIP
	if (!m_isActive) return;		// 起動時フラグOFFならSKIP
	m_emitterCountTime += 1.0f;	// カウント増加
	if (m_durationTime < m_emitterCountTime) {		// エミッター起動時間を超えたらbreak;
		if (m_isLooping) {							// ループフラグONの場合初期化
			m_emitterCountTime = 0;
		}
		return;
	}
    if (m_delayTime > m_emitterCountTime) {	// 遅延時間以内ならSKIP
        return;
    }
}

void Emitter::Draw() {}

void Emitter::SetEmitter() {}

//void Emitter::SetAnimation(AnimationMode animationMode, int sizeX, int sizeY) {
//	m_animationMode = animationMode;
//	m_animationSize = XMINT2(sizeX, sizeY);
//}

void Emitter::CreateParticle(const char* passName) {
    SAFE_RELEASE(m_texture);
    CreateTextureFromFile(DirectGraphics::GetInstance().GetDevice(), passName, &(m_texture));	// テクスチャの読み込み
    SetMaxParticle(MAX_PARTICLE);
}

void Emitter::SetTexture(const char* passName) {
    SAFE_RELEASE(m_texture);
    CreateTextureFromFile(DirectGraphics::GetInstance().GetDevice(), passName, &(m_texture));	// テクスチャの読み込み
    m_particleMesh->SetTexture(m_texture);
}

void Emitter::SetMaxParticle(int maxParticle) {
    for (auto particle : m_particleList) {
        SAFE_DELETE(particle);
    }
    m_particleList.clear();
    m_maxParticle = maxParticle;
    for (int i = 0; i < m_maxParticle; ++i) {
        Object* object = new Object(m_parent);
        object->m_transform->m_name = std::to_string(i) + m_parent->m_transform->m_name;
        Particle* particle = object->AddComponentIf <Particle>();
        m_particleList.push_back(object);
    }
}