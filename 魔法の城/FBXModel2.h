/**
 * @file FBXModel2.h
 * @brief アニメーションクラス
 * 参考サイト https://www.tkng45memo.com/fbxanim
 * @author T.Shibata
 * @date 2020/11/26 作成
 */
#pragma once

// <include>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fbxsdk.h>

// <link FBX SDK library>
// -mt(マルチスレッドデバッグ(MTd))
#pragma comment(lib, "libfbxsdk-mt")
#pragma comment(lib, "zlib-mt")
#pragma comment(lib, "libxml2-mt")
#pragma comment(lib, "libfbxsdk")
#pragma comment(lib, "d3dcompiler")
    // <FBX Model class>
class FBX_Model {
private:
    // <一つの頂点情報を格納する構造体>
    struct VERTEX {
        DirectX::XMFLOAT3 Pos;
    };

    // <GPU(シェーダ側)へ送る数値をまとめた構造体>
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

    // <描画>
    void Draw(
        ID3D11DeviceContext* context,
        DirectX::XMMATRIX& world,
        DirectX::XMMATRIX& view,
        DirectX::XMMATRIX& proj);

    // <モデル作成>
    void Create(
        HWND hwnd,
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        ID3D11RenderTargetView* renderTargetView,
        const char* fbxfile_path);

    // <破棄>
    void Destroy();

private:
    void FBX_Import(const char* fbxfile_path);
    void FBX_GetVertex();
    void FBX_SetVertexData(ID3D11Device* device);
};