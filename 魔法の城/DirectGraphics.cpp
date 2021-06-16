#include <stdio.h>
#include <atlstr.h>
#include <tchar.h>
#include <DirectXMath.h>
#include "DirectGraphics.h"
#include "ShaderBase.h"
#include "Window.h"
#include "GraphicsUtility.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"

#pragma comment(lib,"d3d11.lib")

#define BASE_TEXTURE_NAME ("Resources/texture/Grass_Dense_Tint_02_Leaves_A_Basecolor.png")

enum class ShaderModes {
    NORMAL,
    FOG,
    RIM_LIGHT,
    PARTICLE,
    ANIMATION_MODEL,
    UI,
    BOKEH,
    POM,
    WALL_POM,
    BUMP,
    DEPTH_WRITE,
    DEPTH_SHADOW,
};

static std::string vertexShaderPass[] = {
    "Resources/Shader/VertexShader.cso",
    "Resources/Shader/FogVertexShader.cso",
    "Resources/Shader/RimLightVertexShader.cso",
    "Resources/Shader/VertexParticle.cso",
    "Resources/Shader/AnimationVertexShader.cso",
    "Resources/Shader/UIVertexShader.cso",
    "Resources/Shader/VertexShader.cso",
    "Resources/Shader/POMVS.cso",
    "Resources/Shader/WallPomVS.cso",
    "Resources/Shader/BumpVS.cso",
    "Resources/Shader/DepthWriteVertexShader.cso",
    "Resources/Shader/DepthShadowVertexShader.cso",
};

static std::string pixelShaderPass[] = {
    "Resources/Shader/PixelShader.cso",
    "Resources/Shader/FogPixelShader.cso",
    "Resources/Shader/RimLightPixelShader.cso",
    "Resources/Shader/PixelParticle.cso",
    "Resources/Shader/AnimationPixelShader.cso",
    "Resources/Shader/UIPixelShader.cso",
    "Resources/Shader/BokehPixelShader.cso",
    "Resources/Shader/POMPS.cso",
    "Resources/Shader/WallPomPS.cso",
    "Resources/Shader/BumpPS.cso",
    "Resources/Shader/DepthWritePixelShader.cso",
    "Resources/Shader/DepthShadowPixelShader.cso",
};

bool DirectGraphics::Init() {
    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);
    UINT width  = UINT(rect.right - rect.left);
    UINT height = UINT(rect.bottom - rect.top);

	// DeviceとSwapChainの作成
	if (CreateDeviceAndSwapChain() == false) {
		return false;
	}

	// RenderTargetViewの作成
    if (CreateRenderTargetView("Default") == false) { return false; }
    if (CreateRenderTargetView("DepthWrite", width, height, DXGI_FORMAT_R32_FLOAT) == false) { return false; }

	// Depth、StencilViewの作成
	if (CreateDepthAndStencilView("Default",     width, height, DXGI_FORMAT_D24_UNORM_S8_UINT) == false) { return false; }
    if (CreateDepthAndStencilView("DepthWrite",  width, height, DXGI_FORMAT_D24_UNORM_S8_UINT) == false) { return false; }

    // TextureSamplerの作成
    if (CreateTextureSampler("Default",     D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP)  == false) { return false; }
    if (CreateTextureSampler("DepthShadow", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP) == false) { return false; }

    // DepthStencilStateの作成
    if (CreateDepthStencilState() == false) {
        return false;
    }
    // BlendStateの作成
    if (CreateBlendState() == false) {
        return false;
    }
    // RasterizerStateの作成
    if (CreateRasterizerState() == false) {
        return false;
    }

    // 定数バッファ作成
	if (CreateConstantBuffer("Camera",   0, new CameraBuffer,   sizeof(CameraBuffer))   == false) { return false; }
    if (CreateConstantBuffer("Light",    1, new LightBuffer,    sizeof(LightBuffer))    == false) { return false; }
    if (CreateConstantBuffer("World",    2, new WorldBuffer,    sizeof(WorldBuffer))    == false) { return false; }
    if (CreateConstantBuffer("Material", 3, new MaterialBuffer, sizeof(MaterialBuffer)) == false) { return false; }
    if (CreateConstantBuffer("Texture",  4, new TextureBuffer,  sizeof(TextureBuffer))  == false) { return false; }

	// シェーダ作成
    if (CreateShader("NormalModel",    "PNCT", (int)ShaderModes::NORMAL)          == false) { return false; }
    if (CreateShader("Fog",            "PNCT", (int)ShaderModes::FOG)             == false) { return false; }
    if (CreateShader("RimLight",       "PNCT", (int)ShaderModes::RIM_LIGHT)       == false) { return false; }
    if (CreateShader("Particle",       "PNCT", (int)ShaderModes::PARTICLE)        == false) { return false; }
    if (CreateShader("AnimationModel", "P",    (int)ShaderModes::ANIMATION_MODEL) == false) { return false; }
    if (CreateShader("UI",             "PCT",  (int)ShaderModes::UI)              == false) { return false; }
    if (CreateShader("Bokeh",          "PNCT", (int)ShaderModes::BOKEH)           == false) { return false; }
    if (CreateShader("POM",            "PNTT", (int)ShaderModes::POM)             == false) { return false; }
    if (CreateShader("WallPom",        "PNTT", (int)ShaderModes::WALL_POM)        == false) { return false; }
    if (CreateShader("Bump",           "PNTT", (int)ShaderModes::BUMP)            == false) { return false; }
    if (CreateShader("DepthWrite",     "PNCT", (int)ShaderModes::DEPTH_WRITE)     == false) { return false; }
    if (CreateShader("DepthShadow",    "PNCT", (int)ShaderModes::DEPTH_SHADOW)    == false) { return false; }

    // ViewPort設定
    SetUpdateViewPort(width, height);

	// 変換行列設定
	SetUpTransform();
	return true;
}

void DirectGraphics::Release() {
    MainCamera::GetInstance().Uninit();
    SAFE_RELEASE(m_baseTexture);

    for (int i = 0; i < (int)BlendStates::MAX; ++i) {
        SAFE_RELEASE(m_BlendState[i]);
    }
    for (int i = 0; i < (int)CullModes::MAX; ++i) {
        SAFE_RELEASE(m_RasterizerState[i]);
    }
    for (auto itr = m_InputLayout.begin(); itr != m_InputLayout.end(); itr++) {
        auto layout = itr->second;
        SAFE_RELEASE(layout);
    }
    for (auto itr = m_VertexShader.begin(); itr != m_VertexShader.end();itr++) {
        auto vertex = itr->second;
        SAFE_DELETE(vertex);
    }
    for (auto itr = m_PixelShader.begin(); itr != m_PixelShader.end(); itr++) {
        auto pixel = itr->second;
        SAFE_DELETE(pixel);
    }
    for (auto itr = m_ConstantBuffer.begin(); itr != m_ConstantBuffer.end(); itr++) {
        auto buffer = itr->second;
        SAFE_RELEASE(buffer);
    }
    for (auto itr = m_ConstantBufferData.begin(); itr != m_ConstantBufferData.end(); itr++) {
        auto data = itr->second;
        SAFE_DELETE(itr->second->m_buffer);
        SAFE_DELETE(data);
    }
    for (auto itr = m_DepthStencilTexture.begin(); itr != m_DepthStencilTexture.end(); itr++) {
        auto dst = itr->second;
        SAFE_RELEASE(dst);
    }
    for (auto itr = m_DepthStencilView.begin(); itr != m_DepthStencilView.end(); itr++) {
        auto dsv = itr->second;
        SAFE_RELEASE(dsv);
    }
    for (auto itr = m_RenderTargetTexture.begin(); itr != m_RenderTargetTexture.end(); itr++) {
        auto dst = itr->second;
        SAFE_RELEASE(dst);
    }
    for (auto itr = m_RenderTargetView.begin(); itr != m_RenderTargetView.end(); itr++) {
        auto dsv = itr->second;
        SAFE_RELEASE(dsv);
    }
    SAFE_RELEASE(m_SwapChain);

    if (m_Context != nullptr) {
        m_Context->ClearState();
        m_Context->Flush();
        m_Context->Release();
        m_Context = nullptr;
    }
    SAFE_RELEASE(m_Device);
}

void DirectGraphics::SetRenderTargets(std::string rtvName, std::string dsvName, float *color) {
    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);
    UINT width = UINT(rect.right - rect.left);
    UINT height = UINT(rect.bottom - rect.top);

    // ビューポート設定
    SetUpdateViewPort(width, height);

    // レンダーターゲット設定
    SetUpdateRenderTarget(rtvName, dsvName);
    // レンダー初期化
    if (color) {
        // DirecX9ではRenderTargetとDepth、Stencilバッファのクリアは別々にする
        m_Context->ClearRenderTargetView(
            m_RenderTargetView[rtvName], // クリア対象のView
            (float *)&color); // クリアカラー
        m_Context->ClearDepthStencilView(
            m_DepthStencilView[dsvName],				// クリア対象のView	
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,	// クリアフラグ(今回はDepth、Stencilともにクリア)
            1.0f,										// 深度クリア値
            0);										// ステンシルクリア値
    }
}

void DirectGraphics::StartRendering() {
    // レンダーターゲット設定
    SetUpdateRenderTarget("Default", "Default");

    // DirecX9ではRenderTargetとDepth、Stencilバッファのクリアは別々にする
    m_Context->ClearRenderTargetView(
        m_RenderTargetView["Default"],       // クリア対象のView
        (float *)&m_backgroundColor);        // クリアカラー
    m_Context->ClearDepthStencilView(
        m_DepthStencilView["Default"],             // クリア対象のView    
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,   // クリアフラグ(今回はDepth、Stencilともにクリア)
        1.0f,                                      // 深度クリア値
        0);                                        // ステンシルクリア値
}

void DirectGraphics::FinishRendering() {
	// 描画コマンドを送信する
	m_SwapChain->Present(
			/*
				垂直同期のタイミング
					0なら即時描画
					1以上なら指定した値の垂直同期後に描画
			*/
			1,
			/*
				出力オプション
					フレーム出力を行うためのオプションで
					基本は0で問題ないと思われる
					その他のフラグはDXGI_PRESENTで確認可能
			*/
			0 );	// 出力オプション
}

// Samplerの設定
void DirectGraphics::SetSampler(std::string samplerName) {
    m_Context->PSSetSamplers(0, 1, &m_SamplerState[samplerName]);
}

// Textureの設定
void DirectGraphics::SetTexture(TextureData texture, std::string samplerName) {
    SetSampler(samplerName);
    if (!m_baseTexture) {
        CreateTextureFromFile(m_Device, BASE_TEXTURE_NAME, &m_baseTexture);
    }
    // PixelShaderで使用するテクスチャの設定
    if (texture.texture) {
        m_Context->PSSetShaderResources(texture.Ragister, texture.Num, texture.texture);
    } else {
        m_Context->PSSetShaderResources(texture.Ragister, texture.Num, &m_baseTexture);
    }
}

void DirectGraphics::SetUpTransform() {
	HWND window_handle = FindWindow(Window::ClassName, nullptr);
	RECT rect;
	GetClientRect(window_handle, &rect);
    MainCamera::GetInstance().Init();

	// ライトの設定	
    XMVECTOR lightDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f));
    Light::GetInstance().m_direction = lightDirection;

    SetUpdateCamera();
    SetUpdateLight();
}

// カメラ設定
void DirectGraphics::SetUpdateCamera() {
    MainCamera::GetInstance().Update();
    CameraBuffer* cameraBuffer = (CameraBuffer* )m_ConstantBufferData["Camera"]->m_buffer;    

    XMStoreFloat4x4(&cameraBuffer->m_view, XMMatrixTranspose(XMLoadFloat4x4(&MainCamera::GetInstance().GetView())));
    XMStoreFloat4x4(&cameraBuffer->m_projection, XMMatrixTranspose(XMLoadFloat4x4(&MainCamera::GetInstance().GetProjection())));
    XMStoreFloat4(&cameraBuffer->m_cameraPos,    MainCamera::GetInstance().GetLook());
    XMStoreFloat4(&cameraBuffer->m_cameraVector, MainCamera::GetInstance().GetEye());
    m_Context->UpdateSubresource(m_ConstantBuffer["Camera"], 0, nullptr, cameraBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  0, "Camera");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 0, "Camera");
}

void DirectGraphics::SetUpdateViewProjection(DirectX::XMMATRIX view, DirectX::XMMATRIX projection) {
    CameraBuffer* cameraBuffer = (CameraBuffer*)m_ConstantBufferData["Camera"]->m_buffer;

    XMStoreFloat4x4(&cameraBuffer->m_view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&cameraBuffer->m_projection, XMMatrixTranspose(projection));
    XMStoreFloat4(&cameraBuffer->m_cameraPos, MainCamera::GetInstance().GetLook());
    XMStoreFloat4(&cameraBuffer->m_cameraVector, MainCamera::GetInstance().GetEye());
    m_Context->UpdateSubresource(m_ConstantBuffer["Camera"], 0, nullptr, cameraBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  0, "Camera");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 0, "Camera");
}

void DirectGraphics::SetUpdateViewProjection(DirectX::XMMATRIX view, DirectX::XMMATRIX projection, XMVECTOR pos, XMVECTOR direction) {
    CameraBuffer* cameraBuffer = (CameraBuffer*)m_ConstantBufferData["Camera"]->m_buffer;

    XMStoreFloat4x4(&cameraBuffer->m_view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&cameraBuffer->m_projection, XMMatrixTranspose(projection));
    XMStoreFloat4(&cameraBuffer->m_cameraPos, pos);
    XMStoreFloat4(&cameraBuffer->m_cameraVector, direction);
    m_Context->UpdateSubresource(m_ConstantBuffer["Camera"], 0, nullptr, cameraBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  0, "Camera");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 0, "Camera");
}

// ライトのカラー設定
void DirectGraphics::SetUpdateLight() {
    LightBuffer* lightBuffer = (LightBuffer *)m_ConstantBufferData["Light"]->m_buffer;

    XMStoreFloat4(&lightBuffer->m_pos,    Light::GetInstance().m_position);
    XMStoreFloat4(&lightBuffer->m_vector, Light::GetInstance().m_direction);
    lightBuffer->m_ambient  = *(DirectX::XMFLOAT4 *)&Light::GetInstance().m_ambient;
    lightBuffer->m_diffuse  = *(DirectX::XMFLOAT4 *)&Light::GetInstance().m_diffuse;
    lightBuffer->m_specular = *(DirectX::XMFLOAT4 *)&Light::GetInstance().m_specular;
    XMStoreFloat4x4(&lightBuffer->m_matrix, XMMatrixIdentity());
    m_Context->UpdateSubresource(m_ConstantBuffer["Light"], 0, nullptr, lightBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  1, "Light");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 1, "Light");
}

// ライトのカラー設定
void DirectGraphics::SetUpdateLight(DirectX::XMMATRIX matrix) {
    LightBuffer* lightBuffer = (LightBuffer *)m_ConstantBufferData["Light"]->m_buffer;

    XMStoreFloat4(&lightBuffer->m_pos, Light::GetInstance().m_position);
    XMStoreFloat4(&lightBuffer->m_vector, Light::GetInstance().m_direction);
    lightBuffer->m_ambient = *(DirectX::XMFLOAT4 *)&Light::GetInstance().m_ambient;
    lightBuffer->m_diffuse = *(DirectX::XMFLOAT4 *)&Light::GetInstance().m_diffuse;
    lightBuffer->m_specular = *(DirectX::XMFLOAT4 *)&Light::GetInstance().m_specular;
    XMStoreFloat4x4(&lightBuffer->m_matrix, matrix);
    m_Context->UpdateSubresource(m_ConstantBuffer["Light"], 0, nullptr, lightBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL, 1, "Light");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 1, "Light");
}

// ワールドマトリックス設定
void DirectGraphics::SetUpdateWorldMatrixBuffer(DirectX::XMMATRIX matrix,DirectX::XMMATRIX WVP) {
    WorldBuffer* worldBuffer = (WorldBuffer *)m_ConstantBufferData["World"]->m_buffer;

    XMStoreFloat4x4(&worldBuffer->m_wvp,   XMMatrixTranspose(WVP));
    XMStoreFloat4x4(&worldBuffer->m_world, XMMatrixTranspose(matrix));
    m_Context->UpdateSubresource(m_ConstantBuffer["World"], 0, nullptr, worldBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  2, "World");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 2, "World");
}

// マテリアル設定
void DirectGraphics::SetUpdateMaterial(ObjMaterial material) {
    MaterialBuffer* materialBuffer = (MaterialBuffer *)m_ConstantBufferData["Material"]->m_buffer;

    materialBuffer->m_diffuse  = DirectX::XMFLOAT4(material.Diffuse[0],  material.Diffuse[1],  material.Diffuse[2],  material.Diffuse[3]);
    materialBuffer->m_specular = DirectX::XMFLOAT4(material.Specular[0], material.Specular[1], material.Specular[2], material.Specular[3]);
    materialBuffer->m_ambient  = DirectX::XMFLOAT4(material.Ambient[0],  material.Ambient[1],  material.Ambient[2],  material.Ambient[3]);
    materialBuffer->m_emissive = DirectX::XMFLOAT4(material.Emissive[0], material.Emissive[1], material.Emissive[2], material.Emissive[3]);
    m_Context->UpdateSubresource(m_ConstantBuffer["Material"], 0, nullptr, materialBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  3, "Material");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 3, "Material");
}

// テクスチャ設定
void DirectGraphics::SetUpdateTexture(DirectX::XMMATRIX textureMatrix) {
    TextureBuffer* textureBuffer = (TextureBuffer* )m_ConstantBufferData["Texture"]->m_buffer;
    XMStoreFloat4x4(&textureBuffer->m_tex, XMMatrixTranspose(textureMatrix));
    m_Context->UpdateSubresource(m_ConstantBuffer["Texture"], 0, nullptr, textureBuffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  4, "Texture");
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, 4, "Texture");
}

// その他定数バッファ更新
void DirectGraphics::SetUpdateConstantBuffers(int registerNum,std::string bufferName,void* buffer) {
    m_Context->UpdateSubresource(m_ConstantBuffer[bufferName], 0, nullptr, buffer, 0, 0);
    SetUpdateShaderConstantBuffers(ShaderType::PIXEL,  registerNum, bufferName);
    SetUpdateShaderConstantBuffers(ShaderType::VERTEX, registerNum, bufferName);
}

void DirectGraphics::SetUpdateShaderConstantBuffers(ShaderType shaderType,int registerIndex,std::string bufferName) {
    switch (shaderType) {
        case ShaderType::PIXEL:
            m_Context->PSSetConstantBuffers(registerIndex, 1, &m_ConstantBuffer[bufferName]);
            break;
        case ShaderType::VERTEX:
            m_Context->VSSetConstantBuffers(registerIndex, 1, &m_ConstantBuffer[bufferName]);
            break;
    }
}

// シェーダー名
void DirectGraphics::SetUpdateShader(std::string name) {
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブの形状を指定
	// VerteXShader、PixelShaderを設定
	m_Context->VSSetShader(
		m_VertexShader[name]->GetShaderInterface(),	// 使用するVertexShder
		/*
			ClassInstanceのポインタを設定する
				これはShader作成時のID3D11ClassLinkageを使用した場合に
				用いる項目なので今回はnullptrを指定する
		*/
		nullptr,
		0);									// ClassInstanceの数
	m_Context->PSSetShader(m_PixelShader[name]->GetShaderInterface(), nullptr, 0);
}

void DirectGraphics::SetUpdateRenderTarget(std::string rtvName,std::string dstName) {
    // (OutputManger)RenderTagetの指定
    m_Context->OMSetRenderTargets(
        1,							  // 使用するViewの数
        &m_RenderTargetView[rtvName], // 使用するRenderTargetView
        m_DepthStencilView[dstName]); // 使用するDepthStencilView
}

void DirectGraphics::SetUpdateLayout(std::string layout) {
    m_Context->IASetInputLayout(m_InputLayout[layout]);
}

void DirectGraphics::SetUpDxgiSwapChainDesc(DXGI_SWAP_CHAIN_DESC *dxgi) {
	HWND window_handle = FindWindow(Window::ClassName, nullptr);    
	RECT rect;
	GetClientRect(window_handle, &rect);

	/*
		DirectX11版PresentationParameter
			バッファの数やサイズ、カラーフォーマット等を設定する
	*/
	ZeroMemory(dxgi, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgi->BufferCount = 1;									// バッファの数
	dxgi->BufferDesc.Width   = (rect.right - rect.left);	// バッファの横幅
	dxgi->BufferDesc.Height  = (rect.bottom - rect.top);	// バッファの縦幅
	dxgi->BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;	// カラーフォーマット
	dxgi->BufferDesc.RefreshRate.Numerator = 60;			// リフレッシュレートの分母
	dxgi->BufferDesc.RefreshRate.Denominator = 1;			// リフレッシュレートの分子
	dxgi->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使い方 Usage => 使用方法
	dxgi->OutputWindow       = window_handle;				// 出力対象のウィンドウハンドル
	dxgi->SampleDesc.Count   = 1;							// マルチサンプリングのサンプル数(未使用は1)
	dxgi->SampleDesc.Quality = 0;							// マルチサンプリングの品質(未使用は0)
	dxgi->Windowed = true;									// ウィンドウモード指定
}

bool DirectGraphics::CreateDeviceAndSwapChain() {
	DXGI_SWAP_CHAIN_DESC dxgi;
	SetUpDxgiSwapChainDesc(&dxgi);

	D3D_FEATURE_LEVEL level;
	// デバイス生成とスワップチェーン作成を行う
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,					// ビデオアダプタ指定(nullptrは既定)
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプ
		nullptr,					// D3D_DRIVER_TYPE_SOFTWARE指定時に使用
		0,							// フラグ指定
		nullptr,					// D3D_FEATURE_LEVEL指定で自分で定義した配列を指定可能
		0,							// 上のD3D_FEATURE_LEVEL配列の要素数
		D3D11_SDK_VERSION,			// SDKバージョン
		&dxgi,						// DXGI_SWAP_CHAIN_DESC
		&m_SwapChain,				// 関数成功時のSwapChainの出力先 
		&m_Device,					// 関数成功時のDeviceの出力先
		&level,						// 成功したD3D_FEATURE_LEVELの出力先
		&m_Context)))				// 関数成功時のContextの出力先
	{
		return false;
	}

	return true;
}

bool DirectGraphics::CreateRenderTargetView(std::string rtvName) {
	// RenderTargetViewの対象となるBufferの取得
	ID3D11Texture2D* back_buffer;
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer))) {
		return false;
	}
	// BufferからRenderTargetViewの作成
	if (FAILED(m_Device->CreateRenderTargetView(back_buffer, nullptr, &m_RenderTargetView[rtvName]))) {
		return false;
	}
	// Targetの取得終わったのでBufferを解放
	back_buffer->Release();
	return true;
}


bool DirectGraphics::CreateRenderTargetView(std::string rtvName, UINT width, UINT height, DXGI_FORMAT format) {

    //--- テクスチャ
    // 設定
    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
    texDesc.Usage                = D3D11_USAGE_DEFAULT;
    texDesc.Format               = format;
    texDesc.BindFlags            = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.Width                = width;
    texDesc.Height               = height;
    texDesc.MipLevels            = 1;
    texDesc.ArraySize            = 1;
    texDesc.SampleDesc.Count     = 1;

    // 生成
    ID3D11Texture2D* tex2D;
    if (FAILED(m_Device->CreateTexture2D(&texDesc, NULL, &tex2D))) {
        return false;
    }

    //--- レンダーターゲット
    // 設定
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    // 生成
    if (FAILED(m_Device->CreateRenderTargetView(tex2D, &rtvDesc, &m_RenderTargetView[rtvName]))) {
        return false;
    }

    //--- シェーダリソース
    // 設定
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    // 生成
    if (FAILED(m_Device->CreateShaderResourceView(tex2D, &srvDesc, &m_RenderTargetTexture[rtvName]))) {
        return false;
    }
    return true;
}

bool DirectGraphics::CreateDepthAndStencilView(std::string dsvName, UINT width, UINT height, DXGI_FORMAT format) {
	//深度ステンシルバッファ作成
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory( &texture_desc, sizeof( D3D11_TEXTURE2D_DESC ) );
    texture_desc.Width              = width;			// 横幅
    texture_desc.Height             = height;			// 縦幅
	/*
		ミップマップのレベル指定
			どのレベルまで生成するかという設定
			1はマルチサンプリングされたテクスチャを使用するあるので、
			ミップマップはなしと考えられる
			0は全生成とあるので可能な限りのレベルまでテクスチャが生成されると思われる
	*/
    texture_desc.MipLevels          = 1;
	/*
		テクスチャ配列のサイズ指定
			テクスチャ配列について調べ切れていないので
			他のサンプルと同様に1を設定しておく
	*/
    texture_desc.ArraySize          = 1;
	/*
		テクスチャのフォーマット
			DXGI_FORMAT_D24_UNORM_S8_UINTを使用する
			これはおそらくDepth24bit、Stencil8bitとされると思う
    
			※．UNORMはUnsigned NORMalizedの略で指定された範囲を0.0～1.0にするみたい
	*/
    texture_desc.Format             = format;	// テクスチャーフォーマット
	// マルチサンプリング設定(使わない)
	texture_desc.SampleDesc.Count   = 1;								
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage              = D3D11_USAGE_DEFAULT;				// テクスチャの使用方法(デフォルト)
    texture_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;			// Bind設定はDepth、Stencilに設定
	/*
		リソースへのCPUのアクセス権限についての設定
			ReadとWriteがあるが、権限について現状は考える必要はないはずなので、
			デフォルト値であると思われる0をしておく
    
			※．readとwriteはenum(D3D11_CPU_ACCESS_FLAG)で用意されていた
    
	*/
    texture_desc.CPUAccessFlags     = 0;
	/*
		リソースオプションのフラグ
			Microsoft Docsでフラグを確認する限りは
			通常使用でフラグを設定する必要はないと思われるので
			0としておく
	*/
    texture_desc.MiscFlags          = 0;

	// texture_descの情報でテクスチャを作成
	if (FAILED(m_Device->CreateTexture2D(&texture_desc, nullptr, &m_DepthStencilTexture[dsvName]))) {
		return false;
	}
	// Depth、StencilViewの設定
	// DepthStencilView 
    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory( &dsv_desc, sizeof( dsv_desc ) );
    dsv_desc.Format             = texture_desc.Format;				// Viewのフォーマット(Textureのものを使用)
	/*
		DSVが何次元であるかとTextureの種類を設定する値
			D3D11_DSV_DIMENSION_TEXTURE2Dのほかにも
			D3D11_DSV_DIMENSION_TEXTURE1Dや
			D3D11_DSV_DIMENSION_TEXTURE2D_ARRAYなどがあったので
			DSVが何次元であるかとTextureの種類を設定するメンバと思われる
			今回は何も通常のTextureとして用意しているはず、
			なので、D3D11_DSV_DIMENSION_TEXTURE2Dを指定
	*/
    dsv_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;	
    dsv_desc.Texture2D.MipSlice = 0;								// 最初に使用するミップマップのレベルを指定 
	// CreateTexture2Dとdsv_descからDepthとStencilバッファを作る
	if (FAILED(m_Device->CreateDepthStencilView(
				m_DepthStencilTexture[dsvName],		// DSVとして使用されるTexture
				&dsv_desc,							// DSVの設定
				&m_DepthStencilView[dsvName])))		// ID3D11DepthStencilViewの出力先
	{
		return false;
	}
	return true;
}

bool DirectGraphics::CreateDepthStencilState() {
    CD3D11_DEFAULT def;
    CD3D11_DEPTH_STENCIL_DESC dsd(def);
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    if (FAILED(m_Device->CreateDepthStencilState(&dsd, &m_DepthStencilState[0]))) {
        return false;
    }
    CD3D11_DEPTH_STENCIL_DESC dsd2(def);
    dsd2.DepthEnable = FALSE;
    if (FAILED(m_Device->CreateDepthStencilState(&dsd2, &m_DepthStencilState[1]))) {
        return false;
    }
    return true;
}

bool DirectGraphics::CreateBlendState() {
    // ブレンド ステート生成
    D3D11_BLEND_DESC BlendDesc;
    ZeroMemory(&BlendDesc, sizeof(BlendDesc));
    BlendDesc.AlphaToCoverageEnable = FALSE;
    BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable = FALSE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(m_Device->CreateBlendState(&BlendDesc, &m_BlendState[(int)BlendStates::NONE]))) {
        return false;
    }
    // ブレンド ステート生成 (アルファ ブレンド用)
    //BlendDesc.AlphaToCoverageEnable = TRUE;
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;    
    if (FAILED(m_Device->CreateBlendState(&BlendDesc, &m_BlendState[(int)BlendStates::ALPHABLEND]))) {
        return false;
    }
    // ブレンド ステート生成 (加算合成用)
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    if (FAILED(m_Device->CreateBlendState(&BlendDesc, &m_BlendState[(int)BlendStates::ADDITIVE]))) {
        return false;
    }
    // ブレンド ステート生成 (減算合成用)
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
    if (FAILED(m_Device->CreateBlendState(&BlendDesc, &m_BlendState[(int)BlendStates::SUBTRACTION]))) {
        return false;
    }
    SetBlendState((int)BlendStates::ALPHABLEND);
    return true;
}

bool DirectGraphics::CreateRasterizerState() {
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = TRUE;
    if (FAILED(m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState[0]))) { // 両面描画
        return false;
    }
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
    if (FAILED(m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState[1]))) { // 表面描画
        return false;
    }
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    if (FAILED(m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState[2]))) { // 両面描画
        return false;
    }
    m_Context->RSSetState(m_RasterizerState[0]);
    return true;
}

bool DirectGraphics::CreateInputLayout(std::string layoutName,std::string shaderName, VertexShader *vertex_shader) {
    //頂点レイアウト作成
    if (layoutName == "PNCT" && 
        FAILED(m_Device->CreateInputLayout(INPUT_ELEMENT_PNCT, _countof(INPUT_ELEMENT_PNCT), vertex_shader->GetData(), vertex_shader->GetSize(), &m_InputLayout[shaderName]))) {
        return false;
    } else if(layoutName == "PCT" &&
        FAILED(m_Device->CreateInputLayout(INPUT_ELEMENT_PCT,  _countof(INPUT_ELEMENT_PCT),  vertex_shader->GetData(), vertex_shader->GetSize(), &m_InputLayout[shaderName]))){
        return false;
    } else if (layoutName == "P" &&
        FAILED(m_Device->CreateInputLayout(INPUT_ELEMENT_P,    _countof(INPUT_ELEMENT_P),    vertex_shader->GetData(), vertex_shader->GetSize(), &m_InputLayout[shaderName]))) {
        return false;
    } else if (layoutName == "PNTT" &&
        FAILED(m_Device->CreateInputLayout(INPUT_ELEMENT_PNTT, _countof(INPUT_ELEMENT_PNTT), vertex_shader->GetData(), vertex_shader->GetSize(), &m_InputLayout[shaderName]))) {
        return false;
    }

    return true;
}

bool DirectGraphics::CreateShader(std::string shaderName,std::string layoutName,int shaderIndex) {
	m_VertexShader[shaderName] = new VertexShader();
	if (!m_VertexShader[shaderName]->Create(m_Device, vertexShaderPass[shaderIndex].c_str())) {
		return false;
	}
    CreateInputLayout(layoutName, shaderName,m_VertexShader[shaderName]);
	m_PixelShader[shaderName] = new PixelShader();
	if (!m_PixelShader[shaderName]->Create(m_Device, pixelShaderPass[shaderIndex].c_str())) {
		return false;
	}
	return true;
}

void DirectGraphics::SetUpdateViewPort(UINT width,UINT height) {
	//ビューポートの設定
	D3D11_VIEWPORT view_port;
	view_port.TopLeftX = 0;			    // 左上X座標
	view_port.TopLeftY = 0;			    // 左上Y座標
	view_port.Width    = (FLOAT)width;  // 横幅
	view_port.Height   = (FLOAT)height; // 縦幅
	view_port.MinDepth = 0.0f;			// 最小深度
	view_port.MaxDepth = 1.0f;			// 最大深度
	m_Context->RSSetViewports(	
		1,					// 設定するビューポートの数
		&view_port );		// 設定するビューポート情報のポインタ
}

bool DirectGraphics::CreateTextureSampler(std::string samplerName,D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE adrressMode) {
	D3D11_SAMPLER_DESC sampler_desc;

	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	sampler_desc.Filter = filter;	        // サンプリング時の補間方法
	// UVW値が0.0～1.0の範囲外になった場合の対応設定
	sampler_desc.AddressU = adrressMode;
	sampler_desc.AddressV = adrressMode;
	sampler_desc.AddressW = adrressMode;

	if (FAILED(m_Device->CreateSamplerState(&sampler_desc, &m_SamplerState[samplerName]))) {
		return false;
	}

	return true;
}

bool DirectGraphics::CreateConstantBuffer(std::string bufferName, int registerIndex,void* data,UINT size) {
    // バッファデータ作成
    ConstantBuffer* buffer           = new ConstantBuffer();
    buffer->m_size                   = (float)size;
    buffer->m_buffer                 = data;
    buffer->m_registerIndex          = (float)registerIndex;
    buffer->m_BufferName             = bufferName;
    m_ConstantBufferData[bufferName] = buffer;

	D3D11_BUFFER_DESC buffer_desc;
    ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth           = buffer->GetSize();
	buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags      = 0;
	buffer_desc.MiscFlags           = 0;
	buffer_desc.StructureByteStride = 0;

	if (FAILED(m_Device->CreateBuffer(&buffer_desc, nullptr, &m_ConstantBuffer[bufferName]))) {
		return false;
	}
	return true;
}