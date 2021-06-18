#ifndef ___FONT_H___
#define ___FONT_H___
#include "Polygon.h"

class Font : public Component {
    ID3D11Texture2D          *m_layerBuffer;
    ID3D11ShaderResourceView *m_shaderResourceView;

public:
    Font();
    ~Font();
    void Start();
    void Update();
    void Draw();
};

#endif