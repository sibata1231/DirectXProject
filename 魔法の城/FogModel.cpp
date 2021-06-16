#include "FogModel.h"

#define PATH_FOG_MODEL ("Resources/model/Single Cube 1.fbx") // ƒ‚ƒfƒ‹

FogModel:: FogModel() {}
FogModel::~FogModel() {}

void FogModel::Start() {
    Model* model = m_parent->AddComponentIf<Model>();
    model->Create("FogModel", PATH_FOG_MODEL);
    m_transform->m_name     = "FogModel";
    m_transform->m_tagName  = "FogModel";

    m_renderer = m_parent->GetComponent<ModelRenderer>();
    m_renderer->m_shaderName = "Fog";
    m_parent->m_transform->m_position = Vector3( 0.0f, 0.0f,  0.0f);
    m_parent->m_transform->m_scale    = Vector3(10.0f,10.0f, 10.0f);
    // NormalModel
    // RimLight
    // Bokeh
    // Fog
}

void FogModel::Update() {}

void FogModel::Draw() {

}