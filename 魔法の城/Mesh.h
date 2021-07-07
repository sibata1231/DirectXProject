#ifndef ___MESH_H___
#define ___MESH_H___

#include "Transform.h"
#include "MeshRenderer.h"


// Terrain , Panel
typedef struct {
    Vector3 vtx;		//!< 頂点座標
    Vector3 nor;		//!< 法線ベクトル
    Vector2 tex;		//!< テクスチャ座標
    Vector3 tan;		//!< 接線ベクトル
    UINT     id;        //!< インスタンスID
} POM_VERTEX;

typedef struct {
    Vector3 Position; //!< 頂点座標
    Vector3 Normal;	  //!< 法線ベクトル
    Vector2 UV;		  //!< テクスチャ座標
    Vector3 Tangent;  //!< 接線ベクトル
} TESSELLATION_VERTEX;

// Shadow
typedef struct {
    Vector3 vtx;	//!< 頂点座標
    Vector3 nor;	//!< 法線ベクトル
    Color   color;  //!< カラー
    Vector2 tex;	//!< テクスチャ座標
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