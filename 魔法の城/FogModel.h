#ifndef ___FOG_MODEL_H___
#define ___FOG_MODEL_H___

#include "Model.h"

class FogModel : public Component {
private :
    ModelRenderer *m_renderer;
public:
    FogModel();
    ~FogModel();
    void Start() override;
    void Update()override;
    void Draw()  override;
};

#endif