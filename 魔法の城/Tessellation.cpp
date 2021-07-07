#include "Tessellation.h"
#include "camera.h"
#include "light.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectGraphics.h"
#include <fstream>

void Tessellation::Create(const char * objFile, ID3D11Device * device) {
    // File input object
    std::ifstream obj(objFile);

    // Check for successful open
    if (!obj.is_open()) {
        // Check the debug folder
        char debugFolder[256] = {};
        strcat_s(debugFolder, "Debug/");
        strcat_s(debugFolder, objFile);

        // Attempt to open again
        obj.open(debugFolder);

        // if not found, give up
        if (!obj.is_open())
            return;
    }

    // Variables used while reading the file
    std::vector<Vector3> positions;     // Positions from the file
    std::vector<Vector3> normals;       // Normals from the file
    std::vector<Vector2> uvs;           // UVs from the file
    std::vector<TESSELLATION_VERTEX> verts;           // Verts we're assembling
    std::vector<UINT> indices;           // Indices of these verts
    unsigned int vertCounter = 0;        // Count of vertices/indices
    char chars[100];                     // String for line reading

                                         // Still have data left?
    while (obj.good())
    {
        // Get the line (100 characters should be more than enough)
        obj.getline(chars, 100);

        // Check the type of line
        if (chars[0] == 'v' && chars[1] == 'n')
        {
            // Read the 3 numbers directly into an XMFLOAT3
            Vector3 norm;
            sscanf_s(
                chars,
                "vn %f %f %f",
                &norm.x, &norm.y, &norm.z);

            // Add to the list of normals
            normals.push_back(norm);
        }
        else if (chars[0] == 'v' && chars[1] == 't')
        {
            // Read the 2 numbers directly into an XMFLOAT2
            Vector2 uv;
            sscanf_s(
                chars,
                "vt %f %f",
                &uv.x, &uv.y);

            // Add to the list of uv's
            uvs.push_back(uv);
        }
        else if (chars[0] == 'v')
        {
            // Read the 3 numbers directly into an XMFLOAT3
            Vector3 pos;
            sscanf_s(
                chars,
                "v %f %f %f",
                &pos.x, &pos.y, &pos.z);

            // Add to the positions
            positions.push_back(pos);
        }
        else if (chars[0] == 'f')
        {
            // Read the face indices into an array
            unsigned int i[12];
            int facesRead = sscanf_s(
                chars,
                "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                &i[0], &i[1], &i[2],
                &i[3], &i[4], &i[5],
                &i[6], &i[7], &i[8],
                &i[9], &i[10], &i[11]);

            // - Create the verts by looking up
            //    corresponding data from vectors
            // - OBJ File indices are 1-based, so
            //    they need to be adusted
            TESSELLATION_VERTEX v1;
            v1.Position = positions[i[0] - 1];
            v1.UV = uvs[i[1] - 1];
            v1.Normal = normals[i[2] - 1];

            TESSELLATION_VERTEX v2;
            v2.Position = positions[i[3] - 1];
            v2.UV = uvs[i[4] - 1];
            v2.Normal = normals[i[5] - 1];

            TESSELLATION_VERTEX v3;
            v3.Position = positions[i[6] - 1];
            v3.UV = uvs[i[7] - 1];
            v3.Normal = normals[i[8] - 1];

            // The model is most likely in a right-handed space,
            // especially if it came from Maya.  We want to convert
            // to a left-handed space for DirectX.  This means we 
            // need to:
            //  - Invert the Z position
            //  - Invert the normal's Z
            //  - Flip the winding order
            // We also need to flip the UV coordinate since DirectX
            // defines (0,0) as the top left of the texture, and many
            // 3D modeling packages use the bottom left as (0,0)

            // Flip the UV's since they're probably "upside down"
            v1.UV.y = 1.0f - v1.UV.y;
            v2.UV.y = 1.0f - v2.UV.y;
            v3.UV.y = 1.0f - v3.UV.y;

            // Flip Z (LH vs. RH)
            v1.Position.z *= -1.0f;
            v2.Position.z *= -1.0f;
            v3.Position.z *= -1.0f;

            // Flip normal Z
            v1.Normal.z *= -1.0f;
            v2.Normal.z *= -1.0f;
            v3.Normal.z *= -1.0f;

            // Add the verts to the vector (flipping the winding order)
            verts.push_back(v1);
            verts.push_back(v3);
            verts.push_back(v2);

            // Add three more indices
            indices.push_back(vertCounter); vertCounter += 1;
            indices.push_back(vertCounter); vertCounter += 1;
            indices.push_back(vertCounter); vertCounter += 1;

            // Was there a 4th face?
            if (facesRead == 12)
            {
                // Make the last vertex
                TESSELLATION_VERTEX v4;
                v4.Position = positions[i[9] - 1];
                v4.UV = uvs[i[10] - 1];
                v4.Normal = normals[i[11] - 1];

                // Flip the UV, Z pos and normal
                v4.UV.y = 1.0f - v4.UV.y;
                v4.Position.z *= -1.0f;
                v4.Normal.z *= -1.0f;

                // Add a whole triangle (flipping the winding order)
                verts.push_back(v1);
                verts.push_back(v4);
                verts.push_back(v3);

                // Add three more indices
                indices.push_back(vertCounter); vertCounter += 1;
                indices.push_back(vertCounter); vertCounter += 1;
                indices.push_back(vertCounter); vertCounter += 1;
            }
        }
    }

    // Close the file and create the actual buffers
    obj.close();
    CreateBuffers(&verts[0], vertCounter, &indices[0], vertCounter, device);

}

void Tessellation::CreateBuffers(TESSELLATION_VERTEX * vertices, int numVertices, unsigned int * indices, int numIndices, ID3D11Device * device) {
    // Calculate the tangents before copying to buffer
    CalculateTangents(vertices, numVertices, indices, numIndices);

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(TESSELLATION_VERTEX) * numVertices;       // 3 = number of vertices in the buffer
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// Tells DirectX this is a vertex buffer
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initialVertexData;
    initialVertexData.pSysMem = vertices;

    device->CreateBuffer(&vbd, &initialVertexData, &m_vertexBuffer);


    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned int) * numIndices;         // 3 = number of indices in the buffer
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;					// Tells DirectX this is an index buffer
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initialIndexData;
    initialIndexData.pSysMem = indices;

    m_indexNum = numIndices;

    // Actually create the buffer with the initial data
    // - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
    device->CreateBuffer(&ibd, &initialIndexData, &m_indexBuffer);

    m_transform->m_scale = Vector3(1.0f, 1.0f, 1.0f)*1000.0f;

}
void Tessellation::Start() {
    Mesh::Start();

    // レンダラー初期化
    m_renderer->m_shaderName = "Tessellation";

    // Transform初期化
    m_transform->m_position = Vector3(0.0f,0.0f,0.0f);
    m_transform->m_scale    = Vector3(1.0f,1.0f,1.0f);
    m_transform->UpdateMatrix();

    // コンスタントバッファ Tessellation用 作成
    D3D11_BUFFER_DESC cb;
    ZeroMemory(&cb, sizeof(cb));
    cb.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cb.ByteWidth      = sizeof(TessellationBuffer);
    cb.CPUAccessFlags = 0;
    cb.MiscFlags      = 0;
    cb.Usage          = D3D11_USAGE_DEFAULT;
    DirectGraphics::GetInstance().GetDevice()->CreateBuffer(&cb, nullptr, &m_tessellationBuffer);

    // Tessellation初期化
    m_tessellation.tessellationAmount = 100.0f;
    m_tessellation.Hbias              = 0.0f;
    m_tessellation.Hscale             = 10.0f;
    m_tessellation.time               = 0.0f;
    for (int i = 0; i < _countof(m_tessellation.waves); i++) {
        Wave& w = m_tessellation.waves[i];
        float randomRad = (float)(rand() * XM_PI * 2 * 0.3f);
        w.dir.x = sinf(randomRad);
        w.dir.y = cosf(randomRad);
        float randparam = 2.0f/ rand();
        w.amplitude = 0.03f + powf(2.0f ,2.0f * randparam) * 0.05f;
        w.waveLength = 1.0f + powf(2.f, 1.f) * 10.f;
    }

    // Animationフレーム初期化
    m_animationTime       = 0.0f;
}

void Tessellation::Update() {
    Mesh::Update();
    m_transform->UpdateMatrix();
    static float counter = 0;
    counter += 0.1f;
    m_tessellation.time = counter;
}

void Tessellation::Debug() {
    Mesh::Debug();
    ImGui::Text("[ TessellationAmount : %.3f ]", m_tessellation.tessellationAmount);
    ImGui::Text("[ TessellationHBias  : %.3f ]", m_tessellation.Hbias);
    ImGui::Text("[ TessellationHscale : %.3f ]", m_tessellation.Hscale);

    ImGui::DragFloat("TessellationAmount", &m_tessellation.tessellationAmount, 0.01f);
    ImGui::DragFloat("TessellationHBias", &m_tessellation.Hbias, 0.01f);
    ImGui::DragFloat("TessellationHscale", &m_tessellation.Hscale, 0.01f);
}

void Tessellation::Draw() {
    Mesh::Draw();
    DirectGraphics      *graphics = &DirectGraphics::GetInstance();
	ID3D11DeviceContext *context  = graphics->GetContext();
    MainCamera          *camera   = &MainCamera::GetInstance();

	// 頂点、インデックスバッファ更新
	UINT stride = sizeof(TESSELLATION_VERTEX);
	UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Texture更新
    m_texture = graphics->GetSampleTexture("sphereAlbedo"); 
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL,  DirectGraphics::TextureData(0, 1, &m_texture));

    m_texture = graphics->GetSampleTexture("sphereNormalMap");
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL,  DirectGraphics::TextureData(1, 1, &m_texture));

    m_texture = graphics->GetSampleTexture("sphereHeightMap");
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_DOMAIN, DirectGraphics::TextureData(0, 1, &m_texture));

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

    // Tessellationバッファ更新
    context->UpdateSubresource(m_tessellationBuffer, 0, nullptr, &m_tessellation, 0, 0);
    context->VSSetConstantBuffers(7, 1, &m_tessellationBuffer);
    context->PSSetConstantBuffers(7, 1, &m_tessellationBuffer);
    context->DSSetConstantBuffers(7, 1, &m_tessellationBuffer);
    context->HSSetConstantBuffers(7, 1, &m_tessellationBuffer);

    // 描画設定
    graphics->SetZBuffer(true);
    graphics->SetCullMode((int)CullModes::NONE);

    // 描画
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    context->DrawIndexed(m_indexNum / 2, 6, 0);

    // 描画終了
    graphics->SetZBuffer(false);
    graphics->SetCullMode((int)CullModes::NONE);
}

HRESULT Tessellation::Create(const char* passName, PrimitiveTypes primitiveType, ObjMaterial* material) {
    HRESULT hr      = S_OK;          
    m_vertexNum     = 4;	         // 頂点数の設定
    m_indexNum      = 4;		     // インデックス数の設定
    m_primitiveType = primitiveType; // プリミティブ形状の設定
    hr              = MakeTexture(passName);
    HRESULT_CHECK(hr, "テクスチャ生成失敗");

    // オブジェクトの頂点配列を生成
    TESSELLATION_VERTEX* pVertexWk = new TESSELLATION_VERTEX[m_vertexNum];

    // 頂点バッファの中身を埋める
    TESSELLATION_VERTEX* pVtx = pVertexWk;

    // インデックス配列を生成
    UINT* pIndexWk = new UINT[m_indexNum];

    // 頂点座標の設定
    pVtx[0].Position = Vector3( 1.0f, 1.0f, 0.0f) * m_transform->m_scale + m_transform->m_position;
    pVtx[1].Position = Vector3(-1.0f, 1.0f, 0.0f) * m_transform->m_scale + m_transform->m_position;
    pVtx[2].Position = Vector3( 1.0f,-1.0f, 0.0f) * m_transform->m_scale + m_transform->m_position;
    pVtx[3].Position = Vector3(-1.0f,-1.0f, 0.0f) * m_transform->m_scale + m_transform->m_position;

    // 法線の設定
    pVtx[0].Normal = Vector3(0.0f, 0.0f, 1.0f);
    pVtx[1].Normal = Vector3(0.0f, 0.0f, 1.0f);
    pVtx[2].Normal = Vector3(0.0f, 0.0f, 1.0f);
    pVtx[3].Normal = Vector3(0.0f, 0.0f, 1.0f);

    // テクスチャ座標の設定
    pVtx[0].UV = Vector2(1.0f, 0.0f);
    pVtx[1].UV = Vector2(0.0f, 0.0f);
    pVtx[2].UV = Vector2(1.0f, 1.0f);
    pVtx[3].UV = Vector2(0.0f, 1.0f);
   
    // インデックス配列の中身を埋める
    pIndexWk[0] = 0;
    pIndexWk[1] = 1;
    pIndexWk[2] = 2;
    pIndexWk[3] = 3;

    // タンジェントの設定
    //CalcTangent(pVtx, m_vertexNum, pIndexWk, m_indexNum);

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

    m_transform->m_scale = Vector3(1.0f, 1.0f, 1.0f) * 100.0f;
    return S_OK;
}

// 頂点の作成
HRESULT Tessellation::MakeMeshVertex(TESSELLATION_VERTEX vertexWk[], UINT indexWk[]) {
    ID3D11Device* device = DirectGraphics::GetInstance().GetDevice();

    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage          = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth      = sizeof(TESSELLATION_VERTEX) * m_vertexNum;
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

void Tessellation::CalculateTangents(TESSELLATION_VERTEX* vertex,int numVerts,UINT* indices,int numIndices) {
    // Reset tangents
    for (int i = 0; i < numVerts; i++) {
        vertex[i].Tangent = Vector3(0, 0, 0);
    }

    // Calculate tangents one whole triangle at a time
    for (int i = 0; i < numVerts;)
    {
        // Grab indices and vertices of first triangle
        unsigned int i1 = indices[i++];
        unsigned int i2 = indices[i++];
        unsigned int i3 = indices[i++];
        TESSELLATION_VERTEX* v1 = &vertex[i1];
        TESSELLATION_VERTEX* v2 = &vertex[i2];
        TESSELLATION_VERTEX* v3 = &vertex[i3];

        // Calculate vectors relative to triangle positions
        float x1 = v2->Position.x - v1->Position.x;
        float y1 = v2->Position.y - v1->Position.y;
        float z1 = v2->Position.z - v1->Position.z;

        float x2 = v3->Position.x - v1->Position.x;
        float y2 = v3->Position.y - v1->Position.y;
        float z2 = v3->Position.z - v1->Position.z;

        // Do the same for vectors relative to triangle uv's
        float s1 = v2->UV.x - v1->UV.x;
        float t1 = v2->UV.y - v1->UV.y;

        float s2 = v3->UV.x - v1->UV.x;
        float t2 = v3->UV.y - v1->UV.y;

        // Create vectors for tangent calculation
        float r = 1.0f / (s1 * t2 - s2 * t1);

        float tx = (t2 * x1 - t1 * x2) * r;
        float ty = (t2 * y1 - t1 * y2) * r;
        float tz = (t2 * z1 - t1 * z2) * r;

        // Adjust tangents of each vert of the triangle
        v1->Tangent.x += tx;
        v1->Tangent.y += ty;
        v1->Tangent.z += tz;

        v2->Tangent.x += tx;
        v2->Tangent.y += ty;
        v2->Tangent.z += tz;

        v3->Tangent.x += tx;
        v3->Tangent.y += ty;
        v3->Tangent.z += tz;
    }

    // Ensure all of the tangents are orthogonal to the normals
    for (int i = 0; i < numVerts; i++)
    {
        // Grab the two vectors
        XMVECTOR normal = XMLoadFloat3(&vertex[i].Normal);
        XMVECTOR tangent = XMLoadFloat3(&vertex[i].Tangent);

        // Use Gram-Schmidt orthogonalize
        tangent = XMVector3Normalize(
            tangent - normal * XMVector3Dot(normal, tangent));

        // Store the tangent
        XMStoreFloat3(&vertex[i].Tangent, tangent);
    }
}