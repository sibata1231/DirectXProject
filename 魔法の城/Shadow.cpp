#include "Shadow.h"
#include "camera.h"
#include "light.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectGraphics.h"

HRESULT Shadow::Create(const char* passName, PrimitiveTypes primitiveType, ObjMaterial* material) {
    HRESULT hr      = S_OK;          
    m_vertexNum     = 4;	         // 頂点数の設定
    m_indexNum      = 4;		     // インデックス数の設定
    m_primitiveType = primitiveType; // プリミティブ形状の設定
    hr              = MakeTexture(passName);
    HRESULT_CHECK(hr, "テクスチャ生成失敗");

    // オブジェクトの頂点配列を生成
    SHADOW_VERTEX* pVertexWk = new SHADOW_VERTEX[4];

    // 頂点バッファの中身を埋める
    SHADOW_VERTEX* pVtx = pVertexWk;

    // 頂点座標の設定
    pVtx[0].vtx = XMFLOAT3(-m_transform->m_scale.x * 0.5f, 0.0f, -m_transform->m_scale.y * 0.5f);
    pVtx[1].vtx = XMFLOAT3(-m_transform->m_scale.x * 0.5f, 0.0f,  m_transform->m_scale.y * 0.5f);
    pVtx[2].vtx = XMFLOAT3( m_transform->m_scale.x * 0.5f, 0.0f, -m_transform->m_scale.y * 0.5f);
    pVtx[3].vtx = XMFLOAT3( m_transform->m_scale.x * 0.5f, 0.0f,  m_transform->m_scale.y * 0.5f);

    // 法線の設定
    pVtx[0].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
    pVtx[1].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
    pVtx[2].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
    pVtx[3].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);

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

    // マテリアルの初期設定
    if (material) {
        m_material = material;
    }

    return S_OK;
}

void Shadow::Start() {
    Mesh::Start();

    // Transform初期化
    m_transform->m_name = "Shadow";
    m_transform->m_position = Vector3(0.0f, 0.0f, 0.0f);
    m_transform->m_scale    = Vector3(10.0f, 10.0f, 10.0f);
    m_transform->UpdateMatrix();

    Create("Resources/texture/cloud.jpg");
}

void Shadow::Update() {
    Mesh::Update();
    m_transform->UpdateMatrix();
}

void Shadow::Debug() {
    Mesh::Debug();
}

void Shadow::Draw() {
    Mesh::Draw();
    DirectGraphics      *graphics = &DirectGraphics::GetInstance();
	ID3D11DeviceContext *context  = graphics->GetContext();
    MainCamera          *camera   = &MainCamera::GetInstance();

	// 頂点、インデックスバッファ更新
	UINT stride = sizeof(SHADOW_VERTEX);
	UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // ワールドバッファ更新
	XMMATRIX mtxWorld = XMLoadFloat4x4(&m_transform->m_world);
    XMMATRIX mWVP     = mtxWorld * XMLoadFloat4x4(&camera->GetView()) * XMLoadFloat4x4(&camera->GetProjection());
    graphics->UpdateWorldMatrixBuffer(mtxWorld, mWVP);

    // テクスチャバッファ更新
    graphics->UpdateTexture(XMLoadFloat4x4(&m_mtxTexture));

    // マテリアルバッファ更新
    ObjMaterial material;
    if (m_material) {
        material = *m_material;
    } else {
        material.SetAmbient(M_AMBIENT.x, M_AMBIENT.y, M_AMBIENT.z, M_AMBIENT.w);
        material.SetDiffuse(M_DIFFUSE.x, M_DIFFUSE.y, M_DIFFUSE.z, M_DIFFUSE.w);
        material.SetSpecular(M_SPECULAR.x, M_SPECULAR.y, M_SPECULAR.z, M_SPECULAR.w);
        material.SetEmissive(M_EMISSIVE.x, M_EMISSIVE.y, M_EMISSIVE.z, M_EMISSIVE.w);
    }
    graphics->UpdateMaterial(material);

    // 描画設定
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::FRONT);

    // 描画
    context->DrawIndexed(m_indexNum, 0, 0);

    // 描画終了
    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::NONE);
}

// 頂点の作成
HRESULT Shadow::MakeMeshVertex(SHADOW_VERTEX vertexWk[], int indexWk[]) {
    ID3D11Device* device = DirectGraphics::GetInstance().GetDevice();

    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage          = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth      = sizeof(SHADOW_VERTEX) * m_vertexNum;
    vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags      = 0;

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