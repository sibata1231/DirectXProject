#include "AnimationModel.h"
#include "Camera.h"
#include "Light.h"

AnimationModel::AnimationModel() {
    m_modelManager = &ModelManager::GetInstance();
}

void AnimationModel::Start() {
    m_renderer             = m_parent->AddComponentIf<ModelRenderer>();
    m_renderer->m_isZWrite = true;
    m_isAnimation          = true;
}

void AnimationModel::Update() {
    m_transform->UpdateMatrix();
}

void AnimationModel::Draw() {
    m_modelManager->DrawAnimation(m_animationModelName, m_transform, m_renderer);
}

void AnimationModel::Create(std::string modelName, const char* szPassName) {
    m_modelManager->CreateAnimationModel(modelName, szPassName);
    m_animationModelName = modelName;
}