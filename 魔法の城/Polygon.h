#pragma once

// インクルード
#include "Singleton.h"
#include "RectTransform.h"
#include "SpriteRenderer.h"

typedef struct {
    XMFLOAT3 vtx;		//!< 頂点座標
    XMFLOAT4 diffuse;	//!< 拡散反射光
    XMFLOAT2 tex;		//!< テクスチャ座標
} UI_VERTEX;

#define	NUM_VERTEX		(4)					// 頂点数
class PolygonManager : public Singleton<PolygonManager> {
private:
    ID3D11ShaderResourceView* m_texture;		            // テクスチャへのポインタ
    ID3D11DeviceContext*	  m_deviceContext;              // デバイスへのポインタ
    UI_VERTEX			      m_vertexWk[NUM_VERTEX];		// 頂点情報格納ワーク
    XMFLOAT3				  m_posPolygon;					// ポリゴンの移動量
    XMFLOAT3				  m_rotPolygon;					// ポリゴンの回転量
    XMFLOAT3				  m_sizPolygon;					// ポリゴンのサイズ
    XMFLOAT4X3				  m_matrixVertexPosPolygon;		// ポリゴンの頂点座標
    XMFLOAT4X4				  m_matrixColPolygon;			// ポリゴンの頂点カラー
    XMFLOAT4				  m_matrixFrontTexFramePolygon;	// ポリゴンの頂点UV座標
    XMFLOAT4				  m_matrixBackTexFramePolygon;	// ポリゴンの頂点UV座標
    bool					  m_bInvalidate;				// 頂点データ更新フラグ
                              
    XMFLOAT2				  m_posTexFrame;				// UV座標
    XMFLOAT2				  m_sizTexFrame;				// テクスチャ抽出サイズ
                              
    ID3D11Buffer*			  m_vertexBuffer;				// 頂点バッファ
                                                            
    XMFLOAT4X4				  m_proj;						// 射影変換行列
    XMFLOAT4X4				  m_view;						// ビュー変換行列
    XMFLOAT4X4				  m_world;						// ワールド変換行列
    XMFLOAT4X4				  m_tex;						// テクスチャ変換行列
private:
    void	InitVertexPos();
    void	InitColor();
    void	InitTexFrame();
    void	InitAngle();
    void	InitUV();
    void	InitFrameSize();
    HRESULT MakeVertexPolygon(ID3D11Device* pDevice);
    void    SetVertexPolygon();

public:
    PolygonManager() : m_texture(nullptr) {}
    ~PolygonManager() {};
    void	ResetPolygon();
    void	LoadDeviceContext();
    HRESULT Init(ID3D11Device* pDevice);
    void	Uninit();
    void	Draw();
    void	SetTexture(ID3D11ShaderResourceView *texture);
    void	SetPos(XMFLOAT3 fPos);
    void	SetSize(XMFLOAT2 fSize);
    void	SetAngle(float fAngle);
    void	SetUV(XMFLOAT2 fUV);
    void	SetFrameSize(XMFLOAT2 fFrameSize);
    void	SetVertexPos(XMFLOAT4X3 fPolygonVertexPos);
    void	SetTexFrame(XMFLOAT4 fPolygonFrontTexFrame, XMFLOAT4 fPolygonBackTexFrame);
    void	SetColor(XMFLOAT4 fColor);
    void	SetColor(XMFLOAT4X4 fPolygonColor);
    void	SetAlpha(float fAlpha);
};

// クラス宣言
class Texture : public Component {
private:
    ID3D11ShaderResourceView *m_texture;
    SpriteRenderer           *m_renderer;
public:
    RectTransform *m_rectTransform;
public:
            Texture() : m_texture(nullptr) {}
    		~Texture() {}
    void    Start() override;
    void    Draw()  override;

    HRESULT	Load(const char *szPassName);
    void    Load(ID3D11ShaderResourceView *texture) { m_texture = texture; }
    void	Unload();
private:
    void	SetTexture()   { PolygonManager::GetInstance().SetTexture(m_texture); }
    void	SetPos()       { PolygonManager::GetInstance().SetPos(m_rectTransform->m_position); }
    void	SetSize()      { PolygonManager::GetInstance().SetSize(XMFLOAT2(m_rectTransform->m_scale.x, m_rectTransform->m_scale.y)); }
    void	SetAngle()     { PolygonManager::GetInstance().SetAngle(m_rectTransform->m_rotation.z); }
    void	SetUV()        { PolygonManager::GetInstance().SetUV(m_rectTransform->m_uv); }
    void	SetFrameSize() { PolygonManager::GetInstance().SetFrameSize(m_rectTransform->m_frameSize); }
    void	SetVertexPos() { PolygonManager::GetInstance().SetVertexPos(m_rectTransform->m_polygonVertexPos); }
    void	SetTexFrame()  { PolygonManager::GetInstance().SetTexFrame(m_rectTransform->m_polygonFrontTexFrame, m_rectTransform->m_polygonBackTexFrame); }
    void	SetColor()     { PolygonManager::GetInstance().SetColor(m_rectTransform->m_color); }
    void	SetAlpha()     { PolygonManager::GetInstance().SetAlpha(m_rectTransform->m_color.w); }
};