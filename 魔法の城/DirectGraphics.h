#ifndef ___DIRECT_GRAPHICS_H___
#define ___DIRECT_GRAPHICS_H___

#include <map>
#include <string>

#include <d3d11.h>
#include "Singleton.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GraphicsUtility.h"
#include "imgui.h"
#include "Camera.h"

//=====================================================================//
//! DirectX11の機能を実装するクラス 
//=====================================================================//
class DirectGraphics : public Singleton<DirectGraphics>{
public:
    struct TextureData {
        int Register;
        int Num;
        ID3D11ShaderResourceView** texture;
        TextureData(int Register,int Num, ID3D11ShaderResourceView** texture) {
            this->Register = Register;
            this->Num      = Num;
            this->texture  = texture;
        }
    };

    enum class ShaderType {
        VERTEX,
        PIXEL,
    };
public:
    // 塗りつぶし背景
    ImVec4                    m_backgroundColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);	// RenderTarget塗りつぶしカラー(RGBA)
    ID3D11ShaderResourceView* m_baseTexture;

public:
	/** Destructor */
	~DirectGraphics() {}
	
	/**
	* @brief 初期化関数@n
	* DirectX11の初期化を行い、成功したらtrue、失敗したらfalseを返す
	* @return 初期化の成否 成功(true)
	*/
	bool Init();

	/**
	* @brief DirectX11の解放関数@n
	* 保持しているデバイスなどを解放する
	*/
	void Release();

	/**
	* @brief 描画開始関数@n
	* 描画処理を行う場合、必ずこの関数の後に実行する@n
	* 実行しないと描画されない
	*/
    void StartRendering();

    /**
     * @brief 描画開始関数@n
     * 描画処理を行う場合、必ずこの関数の後に実行する@n
     * 実行しないと描画されない
     */
    void SetRenderTargets(std::string rtvName, std::string dsvName, float* color = nullptr);

	/**
	 * @brief 描画終了関数@n
	 * 描画処理が完了した場合、必ずこの関数を実行する
	 * 実行しないと描画内容が反映されない
	 */
	void FinishRendering();
	
	void SetUpTransform();

    void UpdateShader(std::string name);
    void UpdateRenderTarget(std::string rtvName, std::string dstName);

	ID3D11Device* GetDevice() {
		return m_Device;
	}

	VertexShader *GetVertexShader(std::string name) {
		return m_VertexShader[name];
	}

    PixelShader *GetPixelShader(std::string name) {
        return m_PixelShader[name];
    }

    ID3D11Buffer* GetConstantBuffer(std::string name) {
        return m_ConstantBuffer[name];
	}

	ConstantBuffer* GetConstantBufferData(std::string name) {
		return m_ConstantBufferData[name];
	}

	ID3D11DeviceContext* GetContext() {
		return m_Context;
	}

    ID3D11RenderTargetView* GetRenderTargetView(std::string name) {
        return m_RenderTargetView[name];
    }

    ID3D11ShaderResourceView* GetRenderTargetTexture(std::string name) {
        return m_RenderTargetTexture[name];
    }

    void SetCullMode(int nCullMode) {
        if (nCullMode >= 0 && nCullMode < (int)CullModes::MAX) {
            m_Context->RSSetState(m_RasterizerState[nCullMode]);
        }
    }

    void SetZBuffer(bool isEnable) {        
        m_Context->OMSetDepthStencilState((isEnable) ? nullptr : m_DepthStencilState[1], 0);
    }

    void SetZWrite(bool isEnable) {
        m_Context->OMSetDepthStencilState((isEnable) ? nullptr : m_DepthStencilState[0], 0);
    }
    void SetBlendState(int nBlendState) {
        if (nBlendState >= 0 && nBlendState < (int)BlendStates::MAX) {
            float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            m_Context->OMSetBlendState(m_BlendState[nBlendState], blendFactor, 0xffffffff);
        }
    }
    void SetSampler(std::string samplerName);
    void SetTexture(TextureData texture, std::string samplerName = "Default");

    // コンスタントバッファ
    void UpdateCamera();                                                             // 0
    void UpdateViewProjection(DirectX::XMMATRIX view, DirectX::XMMATRIX projection); // 0
    void UpdateViewProjection(DirectX::XMMATRIX view, DirectX::XMMATRIX projection, XMVECTOR pos, XMVECTOR direction); // 0
    void UpdateLight();                                                              // 1
    void UpdateLight(DirectX::XMMATRIX matrix);                                      // 1
    void UpdateWorldMatrixBuffer(DirectX::XMMATRIX world, DirectX::XMMATRIX WVP);    // 2
    void UpdateMaterial(ObjMaterial material);                                       // 3
    void UpdateTexture(DirectX::XMMATRIX textureMatrix);                             // 4
    void UpdateConstantBuffers(int registerNum, std::string bufferName,void* buffer);// その他
    void UpdateShaderConstantBuffers(ShaderType shaderType, int registerIndex, std::string bufferName); // 定数バッファ更新

    // シェーダー関連
    void UpdateLayout(std::string layout);

private:
	/**
	 * @brief DXGI_SWAP_CHAIN_DESCの設定関数@n
	 * SwapChainを作成するうえで必要な設定をDXGI_SWAP_CHAIN_DESCに行う
	 * @param[out] dxgi 設定を行うDXGI_SWAP_CHAIN_DESCのポインタ
	 */
	void SetUpDxgiSwapChainDesc(DXGI_SWAP_CHAIN_DESC* dxgi);

	/**
	 * @brief DeviceとSwapChainの作成関数@n
	 * DirectX11のDeviceとSwapChainを作成する@n
	 * まとめた理由は生成関数をD3D11CreateDeviceAndSwapChainにしたため
	 * @return 作成の成否 成功(true)
	 */
	bool CreateDeviceAndSwapChain();

	/**
	 * @brief RenderTargetViewの作成関数@n
	 * DirectX11のRenderTargetViewを作成する
	 * @return 作成の成否 成功(true)
	 */
    bool CreateRenderTargetView(std::string rtvName);
    bool CreateRenderTargetView(std::string rtvName, UINT width, UINT height, DXGI_FORMAT format);

	/**
	 * @brief DepthバッファStencilバッファ作成関数
	 * @return 作成の成否 成功(true)
	 */
	//bool CreateDepthAndStencilView();
    bool CreateDepthAndStencilView(std::string dsvName, UINT width, UINT height, DXGI_FORMAT format);

    /**
     * @brief ブレンドステート作成関数
     * @return 作成の成否 成功(true)
     */
    bool CreateBlendState();

    /**
     * @brief ラスターライザステート作成関数
     * @return 作成の成否 成功(true)
     */
    bool CreateRasterizerState();

    /**
     * @brief デプスステンシルステート作成関数
     * @return 作成の成否 成功(true)
     */
    bool CreateDepthStencilState();

	/**
	 * @brief シェーダ作成関数@n
	 * 今回のプロジェクトで使用するシェーダを作成する@n
	 * @return 作成の成否 成功(true)
	 */
    bool CreateShader(std::string shaderName, std::string layoutName, int shaderIndex);

	/**
	 * @brief ViewPort設定関数@n
	 * ContextにViewPortの設定を行う関数@n
	 * ゲーム中に変更がなければ最初に１度行えば問題ない
	 */
    void UpdateViewPort(UINT width, UINT height);

	/**
	 * @brief TextureSampler作成関数@n
	 * TextureSamplerの設定を行い、ID3D11SamplerStateを作成する
	 * @return 作成の成否 成功(true)
	 */
    bool CreateTextureSampler(std::string samplerName,D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE adrressMode);

	/**
	 * @brief ConstantBuffer作成関数@n
	 * TextureSamplerの設定を行い、ID3D11SamplerStateを作成する
	 * @return 作成の成否 成功(true)
	 */
    bool CreateConstantBuffer(std::string bufferName, int registerIndex, void* data, UINT size);

    /**
     * @brief InputLayout作成関数@n
     * VertexShaderからInputLayoutを作成する
     * @return 作成の成否 成功(true)
     */
    bool CreateInputLayout(std::string layoutName, std::string shaderName, VertexShader *vertex_shader);

private:
	ID3D11Device*           m_Device;				        //!< @brief DirectX11のDeviceのInterface
	ID3D11DeviceContext*    m_Context;				        //!< @brief Context
	IDXGISwapChain*         m_SwapChain;			        //!< @brief SwapChainのInterface

    // Render
    std::map<std::string , ID3D11ShaderResourceView*> m_RenderTargetTexture; //!< @brief RenderTargetView生成時に発生するTexture確保用
	std::map<std::string , ID3D11RenderTargetView*  > m_RenderTargetView;	 //!< @brief RenderTargetViewのInterface
	std::map<std::string , ID3D11Texture2D*>          m_DepthStencilTexture; //!< @brief ID3D11DepthStencilViewを生成するためのテクスチャ
	std::map<std::string , ID3D11DepthStencilView*>   m_DepthStencilView;	 //!< @brief DepthStencilViewのInterface

    // Texture
    std::map<std::string , ID3D11SamplerState*>       m_SamplerState;	     //!< @brief Textureサンプラー

    // Shader
    std::map<std::string, VertexShader *>      m_VertexShader;	     //!< @brief VertexShader保持用
    std::map<std::string, PixelShader *>       m_PixelShader;	     //!< @brief PixelShader保持用
    std::map<std::string, ConstantBuffer *>    m_ConstantBufferData; //!< @brief ConstcantBuffer保持用
    std::map<std::string, ID3D11Buffer* >      m_ConstantBuffer;     //!< @brief 定数バッファ保持用
    std::map<std::string, ID3D11InputLayout* > m_InputLayout;	     //!< @brief 入力レイアウト保持用

    // State
    ID3D11RasterizerState*   m_RasterizerState[(int)CullModes::MAX]; //!< @brief RasterizerState保持用
    ID3D11DepthStencilState* m_DepthStencilState[2];                 //!< @brief DepthStencilState保持用
    ID3D11BlendState*		 m_BlendState[(int)BlendStates::MAX];    //!< @brief BlendState
private:
    const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_PNCT[4] = {
        { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_PCT[3] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_P[1] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_PNTT[4] = {
        { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
};

#endif