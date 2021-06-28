#include "polygon.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectGraphics.h"

HRESULT PolygonManager::Init(ID3D11Device* device) {
	HRESULT hr = S_OK;

    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);

	// 変換行列初期化
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
	XMStoreFloat4x4(&m_view,XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), 
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_proj,XMMatrixOrthographicLH((float)rect.right - rect.left, (float)(rect.bottom - rect.top), 1.0f, 100.0f));
	XMStoreFloat4x4(&m_tex, XMMatrixIdentity());
	m_tex._44 = 0.0f;
	m_posPolygon = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotPolygon = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_sizPolygon = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_matrixVertexPosPolygon = XMFLOAT4X3(
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f);
	m_matrixFrontTexFramePolygon = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	m_matrixBackTexFramePolygon  = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	m_matrixColPolygon = XMFLOAT4X4(
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f);
	m_posTexFrame = XMFLOAT2(0.0f, 0.0f);
	m_sizTexFrame = XMFLOAT2(1.0f, 1.0f);
	m_bInvalidate = false;

	hr = MakeVertexPolygon(device);	// 頂点情報の作成
    HRESULT_CHECK(hr,"頂点作成失敗");

	return hr;
}

void PolygonManager::Uninit( ){
	SAFE_RELEASE(m_vertexBuffer);	// 頂点バッファの解放
}

void PolygonManager::LoadDeviceContext() {
    m_deviceContext = DirectGraphics::GetInstance().GetContext();
}

void PolygonManager::Draw() {
    m_deviceContext = DirectGraphics::GetInstance().GetContext();
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(m_sizPolygon.x, m_sizPolygon.y, 0.0f);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotPolygon.x),
		XMConvertToRadians(m_rotPolygon.y),
		XMConvertToRadians(m_rotPolygon.z));
	// 移動
	mWorld *= XMMatrixTranslation(m_posPolygon.x, m_posPolygon.y, 0.0f);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&m_world, mWorld);

    if (m_texture) {
        // 拡縮
        mWorld = XMMatrixScaling(m_sizTexFrame.x, m_sizTexFrame.y, 1.0f);
        // 移動
        mWorld *= XMMatrixTranslation(m_posTexFrame.x, m_posTexFrame.y, 0.0f);
        // テクスチャ マトリックスに設定
        XMStoreFloat4x4(&m_tex, mWorld);
    } else {
		// テクスチャ無し
		m_tex._44 = 0.0f;
	}

	// 頂点バッファ更新
	SetVertexPolygon();

    DirectGraphics *graphics = &DirectGraphics::GetInstance();
    graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_VERTEX, "UI");
    graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_PIXEL , "UI");
    graphics->UpdateLayout("UI");

	UINT stride = sizeof(UI_VERTEX);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    graphics->SetBlendState((int)BlendStates::ALPHABLEND);
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL, DirectGraphics::TextureData(0, 1, &m_texture));
    graphics->UpdateViewProjection(XMLoadFloat4x4(&m_view), XMLoadFloat4x4(&m_proj));
    graphics->UpdateTexture(XMLoadFloat4x4(&m_tex));
    graphics->UpdateWorldMatrixBuffer(XMLoadFloat4x4(&m_world), XMLoadFloat4x4(&m_world));

	// プリミティブ形状をセット
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    
	// ポリゴンの描画
	m_deviceContext->Draw(NUM_VERTEX, 0);
}

HRESULT PolygonManager::MakeVertexPolygon(ID3D11Device* device) {
	// 頂点座標の設定
	m_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	m_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);

	// 拡散反射光の設定
	for (int i = 0; i < 4; ++i) {
		m_vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	m_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	m_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	m_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	m_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(m_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_vertexWk[0];

	HRESULT hr = device->CreateBuffer(&vbd, &initData, &m_vertexBuffer);
    HRESULT_CHECK(hr, "バッファ作成失敗");

	return hr;
}

void PolygonManager::SetVertexPolygon(void) {
	if (m_bInvalidate) {
		//頂点バッファの中身を埋める
		ID3D11DeviceContext* deviceContext = DirectGraphics::GetInstance().GetContext();
		HRESULT hr = S_OK;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// 頂点座標の設定
			m_vertexWk[0].vtx = XMFLOAT3(m_matrixVertexPosPolygon._11, m_matrixVertexPosPolygon._12, m_matrixVertexPosPolygon._13);
			m_vertexWk[1].vtx = XMFLOAT3(m_matrixVertexPosPolygon._21, m_matrixVertexPosPolygon._22, m_matrixVertexPosPolygon._23);
			m_vertexWk[2].vtx = XMFLOAT3(m_matrixVertexPosPolygon._31, m_matrixVertexPosPolygon._32, m_matrixVertexPosPolygon._33);
			m_vertexWk[3].vtx = XMFLOAT3(m_matrixVertexPosPolygon._41, m_matrixVertexPosPolygon._42, m_matrixVertexPosPolygon._43);

			// 拡散反射光の設定
			m_vertexWk[0].diffuse = XMFLOAT4(m_matrixColPolygon._11, m_matrixColPolygon._12, m_matrixColPolygon._13, m_matrixColPolygon._14);
			m_vertexWk[1].diffuse = XMFLOAT4(m_matrixColPolygon._21, m_matrixColPolygon._22, m_matrixColPolygon._23, m_matrixColPolygon._24);
			m_vertexWk[2].diffuse = XMFLOAT4(m_matrixColPolygon._31, m_matrixColPolygon._32, m_matrixColPolygon._33, m_matrixColPolygon._34);
			m_vertexWk[3].diffuse = XMFLOAT4(m_matrixColPolygon._41, m_matrixColPolygon._42, m_matrixColPolygon._43, m_matrixColPolygon._44);

			// テクスチャ座標の設定
			m_vertexWk[0].tex = XMFLOAT2(m_matrixFrontTexFramePolygon.x, m_matrixFrontTexFramePolygon.y);
			m_vertexWk[1].tex = XMFLOAT2(m_matrixFrontTexFramePolygon.z, m_matrixFrontTexFramePolygon.w);
			m_vertexWk[2].tex = XMFLOAT2(m_matrixBackTexFramePolygon.x, m_matrixBackTexFramePolygon.y);
			m_vertexWk[3].tex = XMFLOAT2(m_matrixBackTexFramePolygon.z, m_matrixBackTexFramePolygon.w);

			// 頂点データを上書き
			memcpy_s(msr.pData, sizeof(m_vertexWk), m_vertexWk, sizeof(m_vertexWk));
			// 頂点データをアンロックする
			deviceContext->Unmap(m_vertexBuffer, 0);
			// フラグをクリア
			m_bInvalidate = false;
		}
	}
}

void PolygonManager::SetTexture(ID3D11ShaderResourceView *texture) {
	if (m_texture != texture) {
		m_texture = texture;
		m_tex._44 = (m_texture) ? 1.0f : 0.0f;
	}
	LoadDeviceContext();
}

void PolygonManager::ResetPolygon() {
	PolygonManager::InitVertexPos();
	PolygonManager::InitColor();
	PolygonManager::InitTexFrame();
	PolygonManager::InitAngle();
	PolygonManager::InitUV();
	PolygonManager::InitFrameSize();
}

void PolygonManager::SetPos(XMFLOAT3 fPos) {
	if (m_posPolygon.x != fPos.x ||
		m_posPolygon.y != fPos.y ||
		m_posPolygon.z != fPos.z) {
		m_posPolygon = fPos;
	}
}

void PolygonManager::SetSize(XMFLOAT2 fSize) {
	if (m_sizPolygon.x != fSize.x ||
		m_sizPolygon.y != fSize.y) {
		m_sizPolygon.x = fSize.x;
		m_sizPolygon.y = fSize.y;
	}
}

void PolygonManager::SetAngle(float fAngle) {
	if (m_rotPolygon.z != fAngle) {
		m_rotPolygon.z = fAngle;
	}
}

void PolygonManager::InitAngle() {
	if (m_rotPolygon.z == 0.0f) return;
	m_rotPolygon.z = 0.0f;
}

void PolygonManager::SetUV(XMFLOAT2 fUV) {
	if (m_posTexFrame.x != fUV.x ||
		m_posTexFrame.y != fUV.y) {
		m_posTexFrame = fUV;
	}
}

void PolygonManager::InitUV() {
	if (m_posTexFrame.x == 0.0f ||
		m_posTexFrame.y == 0.0f) {
		m_posTexFrame = XMFLOAT2(0.0f, 0.0f);
	}
}

void PolygonManager::SetFrameSize(XMFLOAT2 fFrameSize) {
	if (m_sizTexFrame.x != fFrameSize.x ||
		m_sizTexFrame.y != fFrameSize.y) {
		m_sizTexFrame = fFrameSize;
	}
}

void PolygonManager::InitFrameSize() {
	if (m_sizTexFrame.x == 1.0f || m_sizTexFrame.y == 1.0f) return;
		m_sizTexFrame = XMFLOAT2(1.0f, 1.0f);
}

void PolygonManager::SetVertexPos(XMFLOAT4X3 fPolygonVertexPos) {
    m_matrixVertexPosPolygon = fPolygonVertexPos;
    m_bInvalidate = true;
}

void PolygonManager::InitVertexPos() {
    m_matrixVertexPosPolygon = XMFLOAT4X3(
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f);
    m_bInvalidate = true;

}

void PolygonManager::SetColor(XMFLOAT4 fColor) {
    m_matrixColPolygon._11 = m_matrixColPolygon._21 = m_matrixColPolygon._31 = m_matrixColPolygon._41 = fColor.x;
    m_matrixColPolygon._12 = m_matrixColPolygon._22 = m_matrixColPolygon._32 = m_matrixColPolygon._42 = fColor.y;
    m_matrixColPolygon._13 = m_matrixColPolygon._23 = m_matrixColPolygon._33 = m_matrixColPolygon._43 = fColor.z;
    m_bInvalidate = true;
}

void PolygonManager::SetColor(XMFLOAT4X4 fPolygonColor) {
    m_matrixColPolygon = fPolygonColor;
    m_bInvalidate = true;
}

void PolygonManager::InitColor() {
    m_matrixColPolygon = XMFLOAT4X4(
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f);
    m_bInvalidate = true;
}

void PolygonManager::SetAlpha(float fAlpha) {
    m_matrixColPolygon._14 = m_matrixColPolygon._24 = m_matrixColPolygon._34 = m_matrixColPolygon._44 = fAlpha;
    m_bInvalidate = true;
}

void PolygonManager::SetTexFrame(XMFLOAT4 fPolygonFrontTexFrame , XMFLOAT4 fPolygonBackTexFrame) {
    m_matrixFrontTexFramePolygon = fPolygonFrontTexFrame;
    m_matrixBackTexFramePolygon = fPolygonBackTexFrame;
    m_bInvalidate = true;
}

void PolygonManager::InitTexFrame() {
    m_matrixFrontTexFramePolygon = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
    m_matrixBackTexFramePolygon = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
    m_bInvalidate = true;
}

void Texture::Start() {
    m_rectTransform = m_parent->AddComponentIf<RectTransform>();
    m_renderer      = m_parent->AddComponentIf<SpriteRenderer>();
}

HRESULT Texture::Load(const char * szPassName) {
    return CreateTextureFromFile(DirectGraphics::GetInstance().GetDevice(), szPassName, &(this->m_texture));	// テクスチャの読み込み
}

void Texture::Unload() {
    SAFE_RELEASE(m_texture);
}

void Texture::Draw() {
    SetTexture();
    SetPos();
    SetAngle();
    SetSize();
    SetColor();
    SetAlpha();
    SetUV();
    SetFrameSize();
    DirectGraphics::GetInstance().SetZBuffer(false);
    PolygonManager::GetInstance().Draw();
    DirectGraphics::GetInstance().SetZBuffer(true);
}