#ifndef ___MESH_H___
#define ___MESH_H___

#include "Transform.h"
#include "MeshRenderer.h"


// Terrain , Panel
typedef struct {
    XMFLOAT3 vtx;		//!< 頂点座標
    XMFLOAT3 nor;		//!< 法線ベクトル
    XMFLOAT2 tex;		//!< テクスチャ座標
    XMFLOAT3 tan;		//!< 接線ベクトル
    UINT     id;        //!< インスタンスID
} POM_VERTEX;

// Shadow
typedef struct {
    XMFLOAT3 vtx;	//!< 頂点座標
    XMFLOAT3 nor;	//!< 法線ベクトル
    XMFLOAT4 color; //!< カラー
    XMFLOAT2 tex;	//!< テクスチャ座標
} SHADOW_VERTEX;

class Mesh : public Component {
protected:
    HRESULT MakeTexture(const char*szPassName);
public:
	ID3D11Buffer*                 m_vertexBuffer;	// 頂点バッファインターフェースへのポインタ
	ID3D11Buffer*                 m_indexBuffer;	// インデックスバッファインターフェースへのポインタ
	int                           m_vertexNum;		// 総頂点数	
	int                           m_indexNum;		// 総インデックス数
    XMFLOAT4X4                    m_mtxTexture;		// テクスチャ マトリックス
    PrimitiveTypes                m_primitiveType;	// プリミティブ型
    ObjMaterial*                  m_material;		// マテリアル
    ID3D11ShaderResourceView*     m_texture;		// テクスチャ
    MeshRenderer*                 m_renderer;       // レンダラー
public:
    virtual void Release();

	Mesh();
    virtual ~Mesh();
    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr) { return S_OK; }
    virtual HRESULT Create(ID3D11ShaderResourceView* texture, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr) { return S_OK; }
    virtual void    Start();
    virtual void    Update();
    virtual void    Draw();
    int inline GetVertex() { return m_vertexNum; }
	int inline GetIndex()  { return m_indexNum;  }
    ID3D11ShaderResourceView* GetTexture() { return m_texture; }
    void SetTexture(ID3D11ShaderResourceView* texture) { m_texture = texture; }
};

#endif