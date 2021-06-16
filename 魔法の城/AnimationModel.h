#ifndef ___ANIMATION_MODEL_H___
#define ___ANIMATION_MODEL_H___

#include "ModelManager.h"

class AnimationModel :public Component {
private:
    bool           m_isAnimation;
    ModelManager   *m_modelManager;
    std::string    m_animationModelName;
    ModelRenderer  *m_renderer;
public:
    AnimationModel();
    virtual void Create(std::string modelName, const char* passName);
    virtual void Start();
    virtual void Update();
    virtual void Draw();
};

#endif