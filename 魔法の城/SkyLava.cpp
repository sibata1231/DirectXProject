#include "SkyLava.h"
#include "Camera.h"

// �}�N����`
#define PATH_MODEL_SKY ("Resources/model/sky_Lava.fbx") // ���f��

// �R���X�g���N�^
SkyLava::SkyLava() {}

// �f�X�g���N�^
SkyLava::~SkyLava() {}

void SkyLava::Start() {
    Model* model = m_parent->AddComponentIf<Model>();
    model->Create("SkyLava", PATH_MODEL_SKY);
    m_transform->m_name    = "SkyLava";
    m_transform->m_tagName = "SkyLava";

    ModelRenderer* renderer = m_parent->GetComponent<ModelRenderer>();
    renderer->m_isZWrite    = false;
    renderer->m_rendererTag = RendererTags::BACKGROUND;
    renderer->m_shaderName  = "NormalModel";

}

void SkyLava::Update() {
    m_transform->m_position = MainCamera::GetInstance().GetLook();
}

void SkyLava::SetStatus(int status) {}

void SkyLava::Draw() {}