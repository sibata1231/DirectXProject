#ifndef ___MODEL_MANAGER_H___
#define ___MODEL_MANAGER_H___

#include <map>
#include "FbxMeshFile.h"
#include "FBXModel2.h"
#include "Transform.h"
#include "Singleton.h"
#include "ModelRenderer.h"

class ModelManager : public Singleton<ModelManager> {
private:
    std::map<std::string, FbxMeshFile *> m_modelList;
    std::map<std::string, FBX_Model *>   m_animationModelList;
public:
    // ‹¤’Ê
    ModelManager();
    virtual ~ModelManager() {};
    void Uninit();

    // Normalƒ‚ƒfƒ‹
    void CreateModel(std::string modelName, const char *passName);
    void Draw(std::string modelName, Transform* transform, ModelRenderer* renderer);

    // Animationƒ‚ƒfƒ‹
    void CreateAnimationModel(std::string modelName, const char *passName);
    void DrawAnimation(std::string modelName, Transform* transform, ModelRenderer* renderer);
};

#endif