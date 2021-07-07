#include "ModelManager.h"
#include "Camera.h"
#include "Light.h"

// 共通
ModelManager::ModelManager() {}

void ModelManager::Uninit() {
    for (auto itr = m_modelList.begin(); itr != m_modelList.end(); ++itr) {
        auto *obj = itr->second;
        SAFE_DELETE(obj);
    }
    for (auto itr = m_animationModelList.begin(); itr != m_animationModelList.end(); ++itr) {
        auto *obj = itr->second;
        obj->Destroy();
        SAFE_DELETE(obj);
    }
}






// 通常モデル
void ModelManager::CreateModel(std::string modelName, const char *passName) {
    if (m_modelList[modelName]) {
        return;
    }
    m_modelList[modelName] = new FbxMeshFile();
    m_modelList[modelName]->Load(passName, DirectGraphics::GetInstance().GetDevice());
}

void ModelManager::Draw(std::string modelName, Transform* transform, ModelRenderer* renderer) {
    m_modelList[modelName]->Render(&DirectGraphics::GetInstance(), transform->m_position, transform->m_scale, transform->m_rotation);
}







// アニメーションモデル
void ModelManager::CreateAnimationModel(std::string modelName, const char *passName) {
    if (m_animationModelList[modelName]) {
        return;
    }
    m_animationModelList[modelName] = new FBX_Model();
    m_animationModelList[modelName]->Create(
        FindWindow(Window::ClassName, nullptr),
        DirectGraphics::GetInstance().GetDevice(),
        DirectGraphics::GetInstance().GetContext(),
        DirectGraphics::GetInstance().GetRenderTargetView("Default"),
        passName);
}

// アニメーションモデル
void ModelManager::DrawAnimation(std::string modelName,Transform* transform,ModelRenderer* renderer) {
    XMMATRIX world      = XMLoadFloat4x4(&transform->m_world);
    XMMATRIX view       = XMLoadFloat4x4(&MainCamera::GetInstance().GetView());
    XMMATRIX projection = XMLoadFloat4x4(&MainCamera::GetInstance().GetProjection());
    m_animationModelList[modelName]->Draw(DirectGraphics::GetInstance().GetContext(), world, view, projection);
}
