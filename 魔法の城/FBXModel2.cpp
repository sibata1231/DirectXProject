
#include "FBXModel2.h"
#include "Shader.h"
#include "DirectGraphics.h"

FBX_Model::FBX_Model() {}

FBX_Model::~FBX_Model() {
    Destroy();
}

void FBX_Model::Draw(ID3D11DeviceContext* context, DirectX::XMMATRIX& world, DirectX::XMMATRIX& view, DirectX::XMMATRIX& proj) {    
    // <�I�u�W�F�N�g�̔��f>
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    DirectGraphics *graphics = &DirectGraphics::GetInstance();
    graphics->UpdateShader("AnimationModel");
    graphics->UpdateLayout("AnimationModel");
    context->IASetIndexBuffer(IndBuffer, DXGI_FORMAT_R32_UINT, 0); // �C���f�b�N�X���̎w��

    // ----- Animation -----
    timeCount += FrameTime;
    if (timeCount > stop) {
        timeCount = start;
    }
    // �ړ��A��]�A�g��̂��߂̍s����쐬
    FbxMatrix globalPosition = m_meshNode->EvaluateGlobalTransform(timeCount);
    FbxVector4 t0 = m_meshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 r0 = m_meshNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 s0 = m_meshNode->GetGeometricScaling(FbxNode::eSourcePivot);
    FbxAMatrix geometryOffset = FbxAMatrix(t0, r0, s0);

    // �e���_�Ɋ|���邽�߂̍ŏI�I�ȍs��̔z��
    FbxMatrix *clusterDeformation = new FbxMatrix[m_mesh->GetControlPointsCount()];
    memset(clusterDeformation, 0, sizeof(FbxMatrix) * m_mesh->GetControlPointsCount());

    FbxSkin *skinDeformer = (FbxSkin *)m_mesh->GetDeformer(0, FbxDeformer::eSkin);
    int clusterCount = skinDeformer->GetClusterCount();
    // �e�N���X�^����e���_�ɉe����^���邽�߂̍s��쐬
    for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
        // �N���X�^(�{�[��)�̎��o��
        FbxCluster *cluster = skinDeformer->GetCluster(clusterIndex);
        FbxMatrix vertexTransformMatrix;
        FbxAMatrix referenceGlobalInitPosition;
        FbxAMatrix clusterGlobalInitPosition;
        FbxMatrix clusterGlobalCurrentPosition;
        FbxMatrix clusterRelativeInitPosition;
        FbxMatrix clusterRelativeCurrentPositionInverse;
        cluster->GetTransformMatrix(referenceGlobalInitPosition);
        referenceGlobalInitPosition *= geometryOffset;
        cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
        clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(timeCount);
        clusterRelativeInitPosition = clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition;
        clusterRelativeCurrentPositionInverse = globalPosition.Inverse() * clusterGlobalCurrentPosition;
        vertexTransformMatrix = clusterRelativeCurrentPositionInverse * clusterRelativeInitPosition;
        // ��ō�����s��Ɋe���_���̉e���x(�d��)���|���Ă��ꂼ��ɉ��Z
        for (int i = 0; i < cluster->GetControlPointIndicesCount(); i++) {
            int index = cluster->GetControlPointIndices()[i];
            double weight = cluster->GetControlPointWeights()[i];
            FbxMatrix influence = vertexTransformMatrix * weight;
            clusterDeformation[index] += influence;
        }
    }

    // �ŏI�I�Ȓ��_���W���v�Z��VERTEX�ɕϊ�
    for (int i = 0; i < m_mesh->GetControlPointsCount(); i++) {
        FbxVector4 outVertex = clusterDeformation[i].MultNormalize(m_mesh->GetControlPointAt(i));
        vertices[i].Pos.x = (FLOAT)outVertex[0];
        vertices[i].Pos.y = (FLOAT)outVertex[1];
        vertices[i].Pos.z = (FLOAT)outVertex[2];
    }

    delete[] clusterDeformation;

    // ---------------------

    D3D11_MAPPED_SUBRESOURCE pdata;
    CONSTANT_BUFFER cb;

    // �p�����[�^�̎󂯓n��(�萔)
    cb.mWVP = DirectX::XMMatrixTranspose(world * view * proj);
    //context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
    //memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));
    //context->Unmap(m_constantBuffer, 0);
    //context->VSSetConstantBuffers(0, 1, &m_constantBuffer); // ���_�V�F�[�_�[�̒萔�o�b�t�@�w��
    //context->PSSetConstantBuffers(0, 1, &m_constantBuffer); // �s�N�Z���V�F�[�_�[�̒萔�o�b�t�@�w��
    graphics->UpdateWorldMatrixBuffer(world, cb.mWVP);

    // �p�����[�^�̎󂯓n��(���_)
    context->Map(VerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
    memcpy_s(pdata.pData, pdata.RowPitch, (void*)(vertices), sizeof(VERTEX) * m_mesh->GetControlPointsCount());
    context->Unmap(VerBuffer, 0);
    context->IASetVertexBuffers(0, 1, &VerBuffer, &stride, &offset);     // ���_���̎w��

    //D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); // �v���~�e�B�u�̎w��
    context->DrawIndexed(m_mesh->GetPolygonVertexCount(), 0, 0);         // �`����s

}

void FBX_Model::Create(
    HWND hwnd,
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    ID3D11RenderTargetView* renderTargetView,
    const char* fbxfile_path) {
    // <�萔�o�b�t�@�̐ݒ�>
    D3D11_BUFFER_DESC cb;
    cb.ByteWidth           = sizeof(CONSTANT_BUFFER);
    cb.Usage               = D3D11_USAGE_DYNAMIC;
    cb.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    cb.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    cb.MiscFlags           = 0;
    cb.StructureByteStride = 0;
    device->CreateBuffer(&cb, nullptr, &m_constantBuffer);

    // <FBX�ǂݍ���>
    FBX_Import(fbxfile_path);
    // <���_�f�[�^�̎��o��>
    //FBX_GetVertex();
    // <���_�f�[�^�p�o�b�t�@�̐ݒ�>
    FBX_SetVertexData(device);
}

void FBX_Model::Destroy() {
    if (m_fbxScene) {
        m_fbxScene->Destroy();
    }
    if (m_fbxManager) {
        m_fbxManager->Destroy();
    }
    VerBuffer->Release();
    if (vertices) {
        delete[] vertices;
    }
}

void FBX_Model::FBX_Import(const char* fbxfile_path) {
    // <FBX�ǂݍ���>
    m_fbxManager = FbxManager::Create();
    m_fbxScene = FbxScene::Create(m_fbxManager, "fbxscene");
    FbxString FileName(fbxfile_path);
    FbxImporter *fbxImporter = FbxImporter::Create(m_fbxManager, "imp");
    fbxImporter->Initialize(FileName.Buffer(), -1, m_fbxManager->GetIOSettings());
    fbxImporter->Import(m_fbxScene);
    fbxImporter->Destroy();

    FbxArray<FbxString*> AnimStackNameArray;
    m_fbxScene->FillAnimStackNameArray(AnimStackNameArray);
    FbxAnimStack *AnimationStack = m_fbxScene->FindMember<FbxAnimStack>(AnimStackNameArray[AnimStackNumber]->Buffer());
    m_fbxScene->SetCurrentAnimationStack(AnimationStack);

    FbxTakeInfo *takeInfo = m_fbxScene->GetTakeInfo(*(AnimStackNameArray[AnimStackNumber]));
    start = takeInfo->mLocalTimeSpan.GetStart();
    stop = takeInfo->mLocalTimeSpan.GetStop();
    FrameTime.SetTime(0, 0, 0, 1, 0, m_fbxScene->GetGlobalSettings().GetTimeMode());
    timeCount = start;

    for (int i = 0; i < m_fbxScene->GetRootNode()->GetChildCount(); i++) {
        if (m_fbxScene->GetRootNode()->GetChild(i)->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
            m_meshNode = m_fbxScene->GetRootNode()->GetChild(i);
            m_mesh = m_meshNode->GetMesh();
            break;
        }
    }

}

void FBX_Model::FBX_GetVertex() {
    // <���_�f�[�^�̎��o��>
    for (int i = 0; i < m_fbxScene->GetRootNode()->GetChildCount(); i++) {
        if (m_fbxScene->GetRootNode()->GetChild(i)->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
            m_mesh = m_fbxScene->GetRootNode()->GetChild(i)->GetMesh();
            break;
        }
    }
    vertices = new VERTEX[m_mesh->GetControlPointsCount()];
    for (int i = 0; i < m_mesh->GetControlPointsCount(); i++) {
        vertices[i].Pos.x = static_cast<FLOAT>(m_mesh->GetControlPointAt(i)[0]);
        vertices[i].Pos.y = static_cast<FLOAT>(m_mesh->GetControlPointAt(i)[1]);
        vertices[i].Pos.z = static_cast<FLOAT>(m_mesh->GetControlPointAt(i)[2]);
    }
}

void FBX_Model::FBX_SetVertexData(ID3D11Device* device) {
    D3D11_BUFFER_DESC bd_vertex;
    bd_vertex.ByteWidth = sizeof(VERTEX) * m_mesh->GetControlPointsCount();
    bd_vertex.Usage = D3D11_USAGE_DYNAMIC;
    bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd_vertex.MiscFlags = 0;
    bd_vertex.StructureByteStride = 0;
    if (FAILED(device->CreateBuffer(&bd_vertex, NULL, &VerBuffer))) {
        assert(false && "Missing !");
    }
    vertices = new VERTEX[m_mesh->GetControlPointsCount()];

    // �C���f�b�N�X�f�[�^�̎��o���ƃo�b�t�@�̐ݒ�
    D3D11_BUFFER_DESC bd_index;
    bd_index.ByteWidth = sizeof(int) * m_mesh->GetPolygonVertexCount();
    bd_index.Usage = D3D11_USAGE_DEFAULT;
    bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd_index.CPUAccessFlags = 0;
    bd_index.MiscFlags = 0;
    bd_index.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA data_index;
    data_index.pSysMem = m_mesh->GetPolygonVertices();
    (device->CreateBuffer(&bd_index, &data_index, &IndBuffer));
}

