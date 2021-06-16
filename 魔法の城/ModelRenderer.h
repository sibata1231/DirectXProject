#ifndef ___MODEL_RENDERER_H___
#define ___MODEL_RENDERER_H___

#include "Renderer.h"

enum class ShadowTypes {
    NONE,
    TARGET,
};
class ModelRenderer : public Renderer {
public:
    bool         m_isZWrite;   // ZWrite�ݒ�t���O(true : ��������ON,false : ��������OFF)
    ShadowTypes  m_shadowType; // Shadow(mode)
    CullModes    m_cullModes;  // �J�����O�ݒ�(0 : ����,0 : �\��,2 : ����)
    ObjMaterial  m_material;
public:
    virtual void Start();
};

#endif