#include "Mesh.h"
#include "Texture.h"
#include "DirectGraphics.h"

Mesh::Mesh() : m_material(nullptr){}

Mesh::~Mesh() {
    Release();
}

void Mesh::Start() {
    m_renderer = m_parent->AddComponentIf<MeshRenderer>();
}

void Mesh::Update() {}

void Mesh::Draw() {}

// テクスチャ作成 
HRESULT Mesh::MakeTexture(const char*passName) {
    HRESULT hr = CreateTextureFromFile(DirectGraphics::GetInstance().GetDevice(), passName, &m_texture);
	XMStoreFloat4x4(&m_mtxTexture, XMMatrixIdentity());
    return hr;
}

// メッシュ解放
void Mesh::Release() {
	SAFE_RELEASE(m_texture);	  // テクスチャ解放
	SAFE_RELEASE(m_vertexBuffer); // 頂点バッファ解放
    SAFE_RELEASE(m_indexBuffer);  // インデックスバッファ解放
}