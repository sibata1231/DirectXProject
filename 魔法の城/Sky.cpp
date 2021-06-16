#include "Sky.h"
#include "Camera.h"

// �}�N����`
#define PATH_MODEL_SKY ("Resources/model/sky.fbx") // ���f��

// �R���X�g���N�^
Sky::Sky() {}

// �f�X�g���N�^
Sky::~Sky() {}

void Sky::Start() {
    Model* model = m_parent->AddComponentIf<Model>();
    model->Create("Sky", PATH_MODEL_SKY);
    m_transform->m_name    = "Sky";
    m_transform->m_tagName = "Sky";

    ModelRenderer* renderer = m_parent->GetComponent<ModelRenderer>();
    renderer->m_isZWrite    = false;
    renderer->m_rendererTag = RendererTags::BACKGROUND;
    renderer->m_shaderName  = "NormalModel";

}

void Sky::Update() {
    m_transform->m_position = MainCamera::GetInstance().GetLook();
}

void Sky::SetStatus(int status) {}

void Sky::Draw() {}