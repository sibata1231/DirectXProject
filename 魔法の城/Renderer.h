#ifndef ___RENDERER_H___
#define ___RENDERER_H___

#include "Object.h"

#define M_DIFFUSE   (XMFLOAT4(1.0f,1.0f,1.0f,1.0f))
#define M_SPECULAR  (XMFLOAT4(0.0f,0.0f,0.0f,0.0f))
#define M_AMBIENT   (XMFLOAT4(1.0f,1.0f,1.0f,1.0f))
#define M_EMISSIVE  (XMFLOAT4(0.0f,0.0f,0.0f,0.0f))

// íËêîíËã`
enum class PrimitiveTypes {
    UNDEFINED = 0,
    POINT,
    LINE,
    LINESTRIP,
    TRIANGLE,
    TRIANGLESTRIP,

    MAX_PRIMITIVETYPE
};

class Renderer : public Component {
public:
    std::string  m_shaderName;
    RendererTags m_rendererTag;
public:
    virtual void Start() override;
};

#endif