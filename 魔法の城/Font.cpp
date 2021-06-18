#include "Font.h"
#include "DirectGraphics.h"

// コンストラクタ
Font::Font() {}

// デストラクタ
Font::~Font() {
    m_layerBuffer->Release();
    m_shaderResourceView->Release();
}

void Font::Start() {
    static const LPCWSTR fontChar = L"驫";

    // フォントハンドルの設定
    UINT fontSize = 64;
    UINT fontWeight = 1000;
    LPCSTR font = "ＭＳ ゴシック";
    LOGFONT lf = {
        fontSize, 0, 0, 0,
        fontWeight, 0, 0, 0,
        SHIFTJIS_CHARSET,
        OUT_TT_ONLY_PRECIS,
        CLIP_DEFAULT_PRECIS,
        PROOF_QUALITY,
        DEFAULT_PITCH | FF_MODERN,
        (WCHAR)font,
    };
    HFONT hFont = CreateFontIndirect(&lf);
    // 現在のウィンドウに適用
    HDC hdc = GetDC(NULL);
    HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, hFont));
    // デバイスにフォントを持たせないとGetGlyphOutlineW関数はエラーとなる。

    // 出力する文字(一文字だけ)
    UINT code = static_cast<UINT>(*fontChar);

    // 17階調のグレーのグリフビットマップ
    const UINT gradFlag = GGO_GRAY4_BITMAP;

    // ビットマップの設定
    GLYPHMETRICS gm;
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    CONST MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };

    // フォントビットマップを取得
    DWORD size = GetGlyphOutlineW(hdc, code, gradFlag, &gm, 0, NULL, &mat);
    BYTE* pMono = new BYTE[size];
    GetGlyphOutlineW(hdc, code, gradFlag, &gm, size, pMono, &mat);

    // フォントの幅と高さ
    INT fontWidth = gm.gmCellIncX;
    INT fontHeight = tm.tmHeight;

    // レンダーターゲットの設定
    D3D11_TEXTURE2D_DESC rtDesc;
    ZeroMemory(&rtDesc, sizeof(rtDesc));
    rtDesc.Width = fontWidth;
    rtDesc.Height = fontHeight;
    rtDesc.MipLevels = 1;
    rtDesc.ArraySize = 1;
    rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtDesc.SampleDesc.Count = 1;
    rtDesc.SampleDesc.Quality = 0;
    rtDesc.Usage = D3D11_USAGE_DYNAMIC;
    rtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    rtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    rtDesc.MiscFlags = 0;

    // フォント用テクスチャを作成
    DirectGraphics::GetInstance().GetDevice()->CreateTexture2D(&rtDesc, nullptr, &m_layerBuffer);
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = rtDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = rtDesc.MipLevels;

    // シェーダリソースビューを作成
    DirectGraphics::GetInstance().GetDevice()->CreateShaderResourceView(m_layerBuffer, &srvDesc, &m_shaderResourceView);

    // Texture設定
    Texture* texture = m_parent->AddComponentIf<Texture>();
    texture->Load(m_shaderResourceView);

    RectTransform *transform = m_parent->GetComponent<RectTransform>();
    transform->m_name      = "FontTest";
    transform->m_tagName   = "FontTest";

    transform->m_position.x = -450.0f + 128.0f;
    transform->m_position.y = 224.0f;
    transform->m_scale.x    = 128.0f;
    transform->m_scale.y    = 128.0f;
}

void Font::Update() {

}

void Font::Draw() {}