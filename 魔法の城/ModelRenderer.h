#ifndef ___MODEL_RENDERER_H___
#define ___MODEL_RENDERER_H___

#include "Renderer.h"

enum class ShadowTypes {
    NONE,
    TARGET,
};
class ModelRenderer : public Renderer {
public:
    bool         m_isZWrite;   // ZWrite設定フラグ(true : 書き込みON,false : 書き込みOFF)
    ShadowTypes  m_shadowType; // Shadow(mode)
    CullModes    m_cullModes;  // カリング設定(0 : 両面,0 : 表面,2 : 裏面)
    ObjMaterial  m_material;
public:
    virtual void Start();
};

#endif