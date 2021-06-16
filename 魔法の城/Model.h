#ifndef ___MODEL_H___
#define ___MODEL_H___

#include "ModelManager.h"

class Model :public Component {
private:
    bool           m_isAnimation;
    ModelManager   *m_modelManager;
    std::string    m_modelName;
    ModelRenderer  *m_renderer;
public:
    Model();
    virtual void Create(std::string modelName, const char* passName);
    virtual void Start();
    virtual void Update();
    virtual void Draw();
};

#endif