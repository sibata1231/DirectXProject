#include "ParticleMesh.h"
#include "camera.h"
#include "light.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectGraphics.h"
#include "Emitter.h"

HRESULT ParticleMesh::Create(const char* passName, PrimitiveTypes primitiveType, ObjMaterial* material) {
    HRESULT hr      = S_OK;          
    m_vertexNum     = 4;	 // 頂点数の設定
    m_indexNum      = 4;		 // インデックス数の設定
    m_primitiveType = primitiveType; // プリミティブ形状の設定
    hr              = MakeTexture(passName);
    HRESULT_CHECK(hr, "テクスチャ生成失敗");

    // オブジェクトの頂点配列を生成
    MESH_VERTEX* pVertexWk = new MESH_VERTEX[4];

    // 頂点バッファの中身を埋める
    MESH_VERTEX* pVtx = pVertexWk;

    // 頂点座標の設定
    pVtx[0].vtx = XMFLOAT3(-m_transform->m_scale.x * 0.5f, -m_transform->m_scale.y * 0.5f, 0.0f);
    pVtx[1].vtx = XMFLOAT3(-m_transform->m_scale.x * 0.5f,  m_transform->m_scale.y * 0.5f, 0.0f);
    pVtx[2].vtx = XMFLOAT3( m_transform->m_scale.x * 0.5f, -m_transform->m_scale.y * 0.5f, 0.0f);
    pVtx[3].vtx = XMFLOAT3( m_transform->m_scale.x * 0.5f,  m_transform->m_scale.y * 0.5f, 0.0f);

    // 法線の設定
    pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
    pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
    pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
    pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

    // 反射光の設定
    pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    // テクスチャ座標の設定
    pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
    pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
    pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
    pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

    // インデックス配列を生成
    int* pIndexWk = new int[4];

    // インデックス配列の中身を埋める
    pIndexWk[0] = 0;
    pIndexWk[1] = 1;
    pIndexWk[2] = 2;
    pIndexWk[3] = 3;

    // 頂点バッファを生成
    hr = MakeMeshVertex(pVertexWk, pIndexWk);
    HRESULT_CHECK(hr, "頂点バッファ生成失敗");

    // 一時配列の解放
    delete[] pIndexWk;
    delete[] pVertexWk;

    // コンスタントバッファ インスタンシング用 作成
    D3D11_BUFFER_DESC cb;
    ZeroMemory(&cb, sizeof(cb));
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.ByteWidth = sizeof(InstanceBuffer);
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb.MiscFlags = 0;
    cb.Usage = D3D11_USAGE_DYNAMIC;
    DirectGraphics::GetInstance().GetDevice()->CreateBuffer(&cb, nullptr, &m_instanceBuffer);

    // マテリアルの初期設定
    if (material) {
        m_material = material;
    }
    return S_OK;
}

HRESULT ParticleMesh::Create(ID3D11ShaderResourceView *texture, PrimitiveTypes primitiveType, ObjMaterial* material) {
    HRESULT hr      = S_OK;           
    m_vertexNum     = 4;	 // 頂点数
    m_indexNum      = 4;		 // インデックス数
    m_primitiveType = primitiveType; // プリミティブ形状
    m_texture       = texture;       // テクスチャ

    // オブジェクトの頂点配列を生成
    MESH_VERTEX* pVertexWk = new MESH_VERTEX[m_vertexNum];

    // 頂点バッファの中身を埋める
    MESH_VERTEX* pVtx = pVertexWk;

    // 頂点座標の設定
    pVtx[0].vtx = XMFLOAT3(-m_transform->m_scale.x * 0.5f, -m_transform->m_scale.y  * 0.5f, 0.0f);
    pVtx[3].vtx = XMFLOAT3( m_transform->m_scale.x * 0.5f,  m_transform->m_scale.y  * 0.5f, 0.0f);
    pVtx[1].vtx = XMFLOAT3(-m_transform->m_scale.x * 0.5f,  m_transform->m_scale.y  * 0.5f, 0.0f);
    pVtx[2].vtx = XMFLOAT3( m_transform->m_scale.x * 0.5f, -m_transform->m_scale.y  * 0.5f, 0.0f);

    // 法線の設定
    pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
    pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
    pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
    pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

    // 反射光の設定
    pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    // テクスチャ座標の設定
    pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
    pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
    pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
    pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

    // インデックス配列を生成
    int* pIndexWk = new int[m_indexNum];

    // インデックス配列の中身を埋める
    pIndexWk[0] = 0;
    pIndexWk[1] = 1;
    pIndexWk[2] = 2;
    pIndexWk[3] = 3;

    // 頂点バッファを生成
    hr = MakeMeshVertex(pVertexWk, pIndexWk);
    HRESULT_CHECK(hr, "頂点バッファ生成失敗");

    // 一時配列の解放
    delete[] pIndexWk;
    delete[] pVertexWk;

    // マテリアルの初期設定
    if (material) {
        m_material = material;
    }
    return S_OK;
}

void ParticleMesh::Start() {
    Mesh::Start();
    m_renderer->m_shaderName = "Particle";
}

void ParticleMesh::Update() {
    Mesh::Update();
}

void ParticleMesh::Draw() {
    Mesh::Draw();
    DirectGraphics      *graphics = &DirectGraphics::GetInstance();
	ID3D11DeviceContext *context  = graphics->GetContext();
    Emitter             *emitter  = m_parent->GetComponent<Emitter>();

    // シェーダ設定
    graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_VERTEX, m_renderer->m_shaderName);
    graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_PIXEL,  m_renderer->m_shaderName);
    graphics->UpdateLayout(m_renderer->m_shaderName);

	// 頂点、インデックスバッファをセット
	UINT stride = sizeof(MESH_VERTEX);
	UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Texture設定
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL, DirectGraphics::TextureData(0, 1, &m_texture));

    // ビュー変換
	XMMATRIX mtxWorld = XMLoadFloat4x4(&m_transform->m_world);
    graphics->UpdateWorldMatrixBuffer(mtxWorld, mtxWorld);

    // テクスチャ行列更新
    ParticleAnimation anim;
    anim.m_count = 0;
    anim.m_frame = 0;
    anim.m_mode = AnimationMode::ONCE_PLAY;
    anim.m_size = XMINT2(1,1);
    SetVertex(anim);
    graphics->UpdateTexture(XMLoadFloat4x4(&m_mtxTexture));

    // マテリアル更新
    graphics->UpdateMaterial(emitter->GetMaterial());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    graphics->SetBlendState((int)emitter->GetBlendState());
    graphics->SetZBuffer(true);
    graphics->SetCullMode((int)CullModes::NONE);

    // インスタンシングバッファ更新
    D3D11_MAPPED_SUBRESOURCE pData;
    if (SUCCEEDED(context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
        InstanceBuffer instance;
        for (int i = 0; i < MAX_INSTANCE_WIDTH * MAX_INSTANCE_HEIGHT; i++) {            
            instance.m_world[i] = XMMatrixTranspose(XMLoadFloat4x4(&emitter->m_particleList[i]->m_transform->m_world));
        }
        memcpy_s(pData.pData, pData.RowPitch, (void*)&instance, sizeof(InstanceBuffer));
        context->Unmap(m_instanceBuffer, 0);
    }
    context->VSSetConstantBuffers(5, 1, &m_instanceBuffer);
    context->DrawIndexedInstanced(m_indexNum, MAX_INSTANCE_WIDTH * MAX_INSTANCE_HEIGHT, 0, 0, 0);

    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::NONE);
    graphics->SetBlendState((int)BlendStates::NONE);

}

// 頂点の作成
HRESULT ParticleMesh::MakeMeshVertex(MESH_VERTEX vertexWk[], int indexWk[]) {
    ID3D11Device* device = DirectGraphics::GetInstance().GetDevice();

    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(MESH_VERTEX) * m_vertexNum;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = vertexWk;
    HRESULT hr = device->CreateBuffer(&vbd, &initData, &m_vertexBuffer);
    HRESULT_CHECK(hr, "頂点バッファ生成失敗");

    CD3D11_BUFFER_DESC ibd(m_indexNum * sizeof(int), D3D11_BIND_INDEX_BUFFER);
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = indexWk;
    hr = device->CreateBuffer(&ibd, &initData, &m_indexBuffer);
    HRESULT_CHECK(hr, "インデックスバッファ生成失敗");

    return hr;
}

void ParticleMesh::SetVertex(ParticleAnimation anim) {
    XMMATRIX mtxTexture, mtxScale, mtxTranslate;
    mtxTexture = XMMatrixIdentity();	// テクスチャマトリックスの初期化

    XMFLOAT2 flame = XMFLOAT2((1.0f / anim.m_size.x), (1.0f / anim.m_size.y));
    mtxScale = XMMatrixScaling(flame.x, flame.y, 1.0f);	// スケールを反映
    mtxTexture = XMMatrixMultiply(mtxTexture, mtxScale);
    mtxTranslate = XMMatrixTranslation(	// 移動を反映
        (int)anim.m_count % anim.m_size.x * flame.x,
        (int)anim.m_count / anim.m_size.x * flame.y, 0.0f);
    mtxTexture = XMMatrixMultiply(mtxTexture, mtxTranslate);
    XMStoreFloat4x4(&m_mtxTexture, mtxTexture);	// テクスチャマトリックスの設定
}