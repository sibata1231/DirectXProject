#include "FxSnow.h"

#define TEXTURE_NAME ("Resources/texture/Particle04_clear_soft.png")

void FxSnow::Start() {
    Emitter::Start();                                   // 親開始処理
    Emitter::SetGenerate(1);                            // 発生数
    Emitter::SetDuration(10.0f);                        // 発生時間
    Emitter::SetStartLifeTime(2.0f);                    // 消失時間
    Emitter::SetGravityMultiplier(false);               // 重力OFF
    Emitter::SetTexture(TEXTURE_NAME);
    m_isEnable = true;
    m_blendMode = BlendStates::ALPHABLEND;
    m_parent->m_transform->m_name = "FxSnow";

    m_material.SetDiffuse(0.6f, 0.8f, 1.0f, 1.0f);
}

void FxSnow::Update() {
    Emitter::Update();
    SetEmitter();
}

#define AREA_WIDTH 150

void FxSnow::SetEmitter() {
    int count = 0;
    for (auto object : m_particleList) {
        Particle* particle = object->GetComponent<Particle>();
        if (particle->GetActive()) {
            continue;
        }
        // Transform
        particle->SetPosition(Vector3(static_cast<float>(rand() % AREA_WIDTH - rand() % AREA_WIDTH), 100.0f, static_cast<float>(rand() % AREA_WIDTH - rand() % AREA_WIDTH)));
        particle->SetRotation(m_transform->m_rotation);
        float size = 3.0f - static_cast<float>((rand() % 10)) * 0.1f;
        particle->SetSize(Vector3(size, size, size));
        particle->m_transform->UpdateMatrix();

        // Rigidbody
        particle->SetSpeed(Vector3(static_cast<float>(rand() % 2 - rand() % 2), static_cast<float>(-rand() % 2) - 1.0f, static_cast<float>(rand() % 2 - rand() % 2)));
        particle->SetGravity(m_isGravity);

        //Common
        particle->SetLifeTime(m_lifeTime);
        particle->SetActive();

        count++;
        if (count >= m_emitterGenerate) {
            break;
        }
    }
}