/**
 * @file FBXModel2.h
 * @brief �A�j���[�V�����N���X
 * �Q�l�T�C�g https://www.tkng45memo.com/fbxanim
 * @author T.Shibata
 * @date 2020/11/26 �쐬
 */
#pragma once

// <include>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fbxsdk.h>

// <link FBX SDK library>
// -mt(�}���`�X���b�h�f�o�b�O(MTd))
#pragma comment(lib, "libfbxsdk-mt")
#pragma comment(lib, "zlib-mt")
#pragma comment(lib, "libxml2-mt")
#pragma comment(lib, "libfbxsdk")
#pragma comment(lib, "d3dcompiler")
    // <FBX Model class>
class FBX_Model {
private:
    // <��̒��_�����i�[����\����>
    struct VERTEX {
        DirectX::XMFLOAT3 Pos;
    };

    // <GPU(�V�F�[�_��)�֑��鐔�l���܂Ƃ߂��\����>
    struct CONSTANT_BUFFER {
        DirectX::XMMATRIX mWVP;
    };

private:
    ID3D11InputLayout *pVertexLayout;
    ID3D11Buffer *m_constantBuffer;

    FbxManager *m_fbxManager = nullptr;
    FbxScene *m_fbxScene = NULL;
    FbxNode* m_meshNode = NULL;
    FbxMesh *m_mesh = NULL;
    ID3D11Buffer *VerBuffer = NULL;
    ID3D11Buffer *IndBuffer = NULL;
    VERTEX *vertices;

    int AnimStackNumber = 0;
    FbxTime FrameTime, timeCount, start, stop;

public:
    FBX_Model();
    ~FBX_Model();

    // <�`��>
    void Draw(
        ID3D11DeviceContext* context,
        DirectX::XMMATRIX& world,
        DirectX::XMMATRIX& view,
        DirectX::XMMATRIX& proj);

    // <���f���쐬>
    void Create(
        HWND hwnd,
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        ID3D11RenderTargetView* renderTargetView,
        const char* fbxfile_path);

    // <�j��>
    void Destroy();

private:
    void FBX_Import(const char* fbxfile_path);
    void FBX_GetVertex();
    void FBX_SetVertexData(ID3D11Device* device);
};