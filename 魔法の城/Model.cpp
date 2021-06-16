#include "Model.h"
#include "Camera.h"
#include "Light.h"

Model::Model() {
    m_modelManager = &ModelManager::GetInstance();
}

void Model::Start() {
    m_renderer             = m_parent->AddComponentIf<ModelRenderer>();
    m_renderer->m_isZWrite = true;
    m_isAnimation          = true;

}

void Model::Update() {

}

void Model::Draw() {
    DirectGraphics::GetInstance().SetZBuffer(m_renderer->m_isZWrite);
    m_modelManager->Draw(m_modelName, m_transform, m_renderer);
    DirectGraphics::GetInstance().SetZBuffer(true);
}

void Model::Create(std::string modelName, const char* szPassName) {
    m_modelManager->CreateModel(modelName, szPassName);
    m_modelName = modelName;
}