#ifndef ___TOON_MAP___
#define ___TOON_MAP___

#include "SystemMain.h"

class ToonMap {
public:
    ToonMap() {}
    ~ToonMap() {}

    //初期化
    void Init();
    //テクスチャをピクセルシェーダーに設定する、モデルをドローする時に呼んでください
    void SetShaderResourceView();
private:
    ID3D11ShaderResourceView* m_srv = nullptr;
};

#endif
