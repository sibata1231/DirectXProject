#include "ToonMap.h"
#include "DirectGraphics.h"
#include "Texture.h"

void ToonMap::Init() {
    //テクスチャを読み込みます
    CreateTextureFromFile(
        DirectGraphics::GetInstance()->GetDevice(), // D3Dデバイス
        "Resources/texture/toonmap.dds",  //テクスチャのファイルパス
        &m_srv  //シェーダーリソースビュー
    );
}

void ToonMap::SetShaderResourceView() {
    //ピクセルシェーダーにテクスチャを設定します。
    ID3D11ShaderResourceView* srvArray[]{
            m_srv
    };
    DirectGraphics::GetInstance()->GetContext()->PSSetShaderResources(4, 1, srvArray);
}