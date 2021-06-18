#include "Panel.h"
#include "camera.h"
#include "light.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectGraphics.h"
#include "GameData.h"

#define TEST_STAGE_WIDTH  6
#define TEST_STAGE_HEIGHT 5

static int stageData[][TEST_STAGE_HEIGHT][TEST_STAGE_WIDTH] = { 
{
    { 1,1,1,1,1,1,},
    { 1,0,1,0,0,1,},
    { 1,0,1,0,0,1,},
    { 1,0,0,0,0,1,},
    { 1,1,1,1,1,1,},
}, 
{
    { 1,1,1,1,0,0,},
    { 1,1,1,1,0,0,},
    { 1,1,1,1,0,0,},
    { 1,1,1,1,0,0,},
    { 1,1,1,1,0,0,},
} 
};

HRESULT Panel::Create(const char* passName, PrimitiveTypes primitiveType, ObjMaterial* material) {
    HRESULT hr      = S_OK;          
    m_vertexNum     = 4;	         // 頂点数の設定
    m_indexNum      = 4;		     // インデックス数の設定
    m_primitiveType = primitiveType; // プリミティブ形状の設定
    hr              = MakeTexture(passName);
    HRESULT_CHECK(hr, passName);

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

    m_startPosition = m_transform->m_position;

    return S_OK;
}

HRESULT Panel::CreateBumpMapTexture(const char* passName) {
    return CreateTextureFromFile(DirectGraphics::GetInstance().GetDevice(), passName, &m_bumpTexture);
}

void Panel::Start() {
    Mesh::Start();
    m_renderer->m_shaderName = "Bump";

    // コンスタントバッファ POM用 作成
    D3D11_BUFFER_DESC cb;
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

void Panel::Update() {
    Mesh::Update();
    m_transform->UpdateMatrix();
    //m_animationTime += 0.01f;
    m_pom.m_vFrame   = m_animationTime;
}

void Panel::Debug() {
    Mesh::Debug();
    ImGui::Text("[ POM_U : %.3f ]",m_pom.m_uFrame);
    ImGui::SameLine();
    ImGui::Text("[ POM_V : %.3f ]", m_pom.m_vFrame);

    ImGui::Text("[Panel_ID : %d]", m_id);
    ImGui::DragFloat("POM_SPECULAR", &m_pom.m_specularPower, 0.01f);
}

void Panel::Draw() {
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

    // POMバッファ更新
    context->UpdateSubresource(m_pomBuffer, 0, nullptr, &m_pom, 0, 0);
    context->PSSetConstantBuffers(6, 1, &m_pomBuffer);

    // 描画設定
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    graphics->SetZBuffer(true);
    graphics->SetCullMode((int)CullModes::NONE);

    // 描画
    context->Draw(m_indexNum, 0);

    // 描画終了
    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::NONE);
}

// 頂点の作成
HRESULT Panel::MakeMeshVertex(POM_VERTEX vertexWk[], int indexWk[]) {
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

DirectX::XMFLOAT3 Panel::CalcTangent(POM_VERTEX p0, POM_VERTEX p1, POM_VERTEX p2) {
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

void Panel::Reset() {
    m_transform->m_position = m_startPosition;
    m_transform->m_rotation = Vector3(0, 0, 0);
    m_transform->m_object->m_isActive = false;
    m_transform->ResetTween();
}

// PanelManager

// 追加
void PanelManager::Add(Panel* panel) {
    m_map.push_back(panel);
};

void PanelManager::Reset() {
    m_count = 0;
    for (auto itr = m_map.begin(); itr != m_map.end(); itr++) {
        Panel* panel = *itr;
        panel->Reset();
    }
}

void PanelManager::Init(ObjectManager* objectManager) {
    for (int j = 0; j < TEST_STAGE_HEIGHT; j++) {
        for (int i = 0; i < TEST_STAGE_WIDTH; i++) {
            // オブジェクト情報
            Object* object = new Object(nullptr);
            object->m_transform->m_position = Vector3(50.0f * (i - TEST_STAGE_WIDTH / 2), 0.0f, (50.0f *(j - TEST_STAGE_HEIGHT / 2)));
            object->m_transform->m_scale = Vector3(7.0f, 7.0f, 7.0f);

            // パネル情報
            Panel* panel = object->AddComponent<Panel>();
            panel->m_id = i + j * TEST_STAGE_WIDTH;
            panel->Create("Resources/texture/granit-grass.png");
            panel->CreateBumpMapTexture("Resources/texture/granit-grassNormalMap.png");
            Add(panel);

            // 追加処理
            object->m_transform->m_name = "Panel" + std::to_string(panel->m_id);
            object->m_isActive          = false;
            objectManager->Add(object);
        }
    }
}

void PanelManager::CreateStage(int stageNo) {
    Reset();
    int count = 0;
    for (auto itr = m_map.begin(); itr != m_map.end(); itr++, count++) {
        Panel* panel = *itr;
        if (stageData[stageNo][count / TEST_STAGE_WIDTH][(TEST_STAGE_WIDTH - 1 ) - count % TEST_STAGE_WIDTH] == 0) {
            continue;
        }
        m_count++;
        panel->m_transform->m_object->m_isActive = true;
    }
}

XMINT2 PanelManager::StageSize() {
    return XMINT2{ TEST_STAGE_HEIGHT , TEST_STAGE_WIDTH };
}

bool PanelManager::IsCheck(Vector3 pos) {
    Panel* panel = CheckUp(pos);
    if (panel) {
        return panel->m_transform->m_object->m_isActive;
    }
    return false;
}

// 調査
Panel* PanelManager::CheckUp(Vector3 pos) {
    for (auto itr = m_map.begin(); itr != m_map.end(); itr++) {
        Panel* panel = *itr;
        if (pos.x == panel->m_transform->m_position.x &&
            pos.y == panel->m_transform->m_position.y &&
            pos.z == panel->m_transform->m_position.z) {
            return panel;
        }
    }
    return nullptr;
}

// 削除
void PanelManager::Delete(Vector3 pos) {
    for (auto itr = m_map.begin(); itr != m_map.end(); itr++) {
        Panel* panel = *itr;
        if (!panel->m_transform->m_object->m_isActive) {
            continue;
        }
        if (pos.x == panel->m_transform->m_position.x &&
            pos.y == panel->m_transform->m_position.y &&
            pos.z == panel->m_transform->m_position.z) {
            panel->m_transform->DOMoveY(-50.0f, 0.5f)
                                .SetRelative()
                                .OnComplete([](Transform transform) {
                                    transform.m_object->SetActive(false);
                                    PanelManager::GetInstance().m_count--;
                                    if (PanelManager::GetInstance().m_count <= 1) {
                                        GameData::GetInstance().m_stages[0].m_isClear = true;
                                        GameData::GetInstance().Reset();
                                        PanelManager::GetInstance().CreateStage(1);
                                    }
                                });
            panel->m_transform->DORotateY(XM_PI + (3 - rand() % 6), 0.3f);
            break;
        }
    }
}