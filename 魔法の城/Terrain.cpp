#include "Terrain.h"
#include "camera.h"
#include "light.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectGraphics.h"

HRESULT Terrain::Create(const char* passName, PrimitiveTypes primitiveType, ObjMaterial* material) {
    HRESULT hr      = S_OK;          
    m_vertexNum     = 4;	         // 頂点数の設定
    m_indexNum      = 4;		     // インデックス数の設定
    m_primitiveType = primitiveType; // プリミティブ形状の設定
    hr              = MakeTexture(passName);
    HRESULT_CHECK(hr, "テクスチャ生成失敗");

    // オブジェクトの頂点配列を生成
    POM_VERTEX* pVertexWk = new POM_VERTEX[4];

    // 頂点バッファの中身を埋める
    POM_VERTEX* pVtx = pVertexWk;

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

    // タンジェントの設定
    pVtx[0].tan = CalcTangent(pVtx[0], pVtx[1], pVtx[2]);
    pVtx[1].tan = CalcTangent(pVtx[0], pVtx[1], pVtx[2]);
    pVtx[2].tan = CalcTangent(pVtx[0], pVtx[1], pVtx[2]);
    pVtx[3].tan = CalcTangent(pVtx[1], pVtx[2], pVtx[3]);

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

HRESULT Terrain::CreateBumpMapTexture(const char* passName) {
    return CreateTextureFromFile(DirectGraphics::GetInstance().GetDevice(), passName, &m_bumpTexture);
}

void Terrain::Start() {
    Mesh::Start();

    // レンダラー初期化
    m_renderer->m_shaderName = "WallPom";

    // Transform初期化
    m_transform->m_position = Vector3(-81.5f, 0.0f, -60.0f);
    m_transform->m_scale    = Vector3(4.65f, 4.65f, 4.65f);
    m_transform->UpdateMatrix();

    // コンスタントバッファ インスタンシング用 作成
    D3D11_BUFFER_DESC cb;
    ZeroMemory(&cb, sizeof(cb));
    cb.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cb.ByteWidth      = sizeof(InstanceBuffer);
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb.MiscFlags      = 0;
    cb.Usage          = D3D11_USAGE_DYNAMIC;
    DirectGraphics::GetInstance().GetDevice()->CreateBuffer(&cb, nullptr, &m_instanceBuffer);

    // コンスタントバッファ POM用 作成
    ZeroMemory(&cb, sizeof(cb));
    cb.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cb.ByteWidth      = sizeof(POMBuffer);
    cb.CPUAccessFlags = 0;
    cb.MiscFlags      = 0;
    cb.Usage          = D3D11_USAGE_DEFAULT;
    DirectGraphics::GetInstance().GetDevice()->CreateBuffer(&cb, nullptr, &m_pomBuffer);

    // POM初期化
    m_pom.m_specularPower = 1.0f;
    m_pom.m_uFrame        = 0.0f;
    m_pom.m_vFrame        = 0.0f;

    // Animartionフレーム初期化
    m_animationTime       = 0.0f;
}

void Terrain::Update() {
    Mesh::Update();
    m_transform->UpdateMatrix();

    m_animationTime += 0.01f;
    if (m_animationTime >= 1.0f) {
        m_animationTime -= 1.0f;
    }
    m_pom.m_vFrame   = m_animationTime;
}

void Terrain::Debug() {
    Mesh::Debug();
    ImGui::Text("[ POM_U : %.3f ]",m_pom.m_uFrame);
    ImGui::SameLine();
    ImGui::Text("[ POM_V : %.3f ]", m_pom.m_vFrame);
    ImGui::DragFloat("POM_SPECULAR", &m_pom.m_specularPower, 0.01f);
}

void Terrain::Draw() {
    Mesh::Draw();
    DirectGraphics      *graphics = &DirectGraphics::GetInstance();
	ID3D11DeviceContext *context  = graphics->GetContext();
    MainCamera          *camera   = &MainCamera::GetInstance();

	// 頂点、インデックスバッファ更新
	UINT stride = sizeof(POM_VERTEX);
	UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Texture更新
    ID3D11ShaderResourceView* texture[] = {
        m_texture,
        m_bumpTexture,
    };
    graphics->SetTexture(DirectGraphics::TextureData(0, 2, texture));

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

    // インスタンスバッファ更新
    D3D11_MAPPED_SUBRESOURCE pData;
    if (SUCCEEDED(context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
        InstanceBuffer instance;
        for (int j = 0; j < MAX_INSTANCE_HEIGHT; ++j) {
            for (int i = 0; i < MAX_INSTANCE_WIDTH; ++i) {
                XMFLOAT4X4 m = m_transform->m_world;
                m._41 += (i - MAX_INSTANCE_WIDTH)  * 100.0f;
                m._43 += (j - MAX_INSTANCE_HEIGHT) * 100.0f;
                instance.m_world[i + j * MAX_INSTANCE_WIDTH] = XMMatrixTranspose(XMLoadFloat4x4(&m));
            }
        }
        memcpy_s(pData.pData, pData.RowPitch, (void*)&instance, sizeof(InstanceBuffer));
        context->Unmap(m_instanceBuffer, 0);
    }
    context->VSSetConstantBuffers(5, 1, &m_instanceBuffer);

    // POMバッファ更新
    context->UpdateSubresource(m_pomBuffer, 0, nullptr, &m_pom, 0, 0);
    context->PSSetConstantBuffers(6, 1, &m_pomBuffer);

    // 描画設定
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::NONE);

    // 描画
    context->DrawIndexedInstanced(m_indexNum, MAX_INSTANCE_HEIGHT * MAX_INSTANCE_HEIGHT, 0, 0, 0);

    // 描画終了
    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::NONE);
}

// 頂点の作成
HRESULT Terrain::MakeMeshVertex(POM_VERTEX vertexWk[], int indexWk[]) {
    ID3D11Device* device = DirectGraphics::GetInstance().GetDevice();

    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage          = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth      = sizeof(POM_VERTEX) * m_vertexNum;
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

DirectX::XMFLOAT3 Terrain::CalcTangent(POM_VERTEX p0, POM_VERTEX p1, POM_VERTEX p2) {
    // 5次元→3次元頂点に
    DirectX::XMVECTOR vP0[] = {
       DirectX::XMVectorSet(p0.vtx.x, p0.tex.x, p0.tex.y,0),
       DirectX::XMVectorSet(p0.vtx.y, p0.tex.x, p0.tex.y,0),
       DirectX::XMVectorSet(p0.vtx.z, p0.tex.x, p0.tex.y,0),
    };
    DirectX::XMVECTOR vP1[] = {
        DirectX::XMVectorSet(p1.vtx.x, p1.tex.x, p1.tex.y,0),
        DirectX::XMVectorSet(p1.vtx.y, p1.tex.x, p1.tex.y,0),
        DirectX::XMVectorSet(p1.vtx.z, p1.tex.x, p1.tex.y,0),
    };
    DirectX::XMVECTOR vP2[] = {
        DirectX::XMVectorSet(p2.vtx.x, p2.tex.x, p2.tex.y,0),
        DirectX::XMVectorSet(p2.vtx.y, p2.tex.x, p2.tex.y,0),
        DirectX::XMVectorSet(p2.vtx.z, p2.tex.x, p2.tex.y,0),
    };

    float u[3];
    for (int i = 0; i < 3; i++) {
        DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(vP1[i], vP0[i]);
        DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(vP2[i], vP1[i]);
        DirectX::XMVECTOR cross = DirectX::XMVector3Cross(v1, v2);
        DirectX::XMFLOAT3 vec;
        DirectX::XMStoreFloat3(&vec, cross);
        u[i] = -vec.y / vec.x;
    }

    DirectX::XMFLOAT3 tangent;
    tangent.x = u[0];
    tangent.y = u[1];
    tangent.z = u[2];

    return tangent;
}