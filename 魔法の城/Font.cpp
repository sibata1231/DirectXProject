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
    if (FAILED(DirectGraphics::GetInstance().GetDevice()->CreateTexture2D(&rtDesc, nullptr, &m_layerBuffer))) {
        return;
    }

    // フォント用テクスチャリソースにテクスチャ情報をコピー
    D3D11_MAPPED_SUBRESOURCE mappedSubrsrc;
    DirectGraphics::GetInstance().GetContext()->Map(m_layerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubrsrc);
    BYTE* pBits = static_cast<BYTE*>(mappedSubrsrc.pData);
    INT iOfs_x = gm.gmptGlyphOrigin.x;
    INT iOfs_y = tm.tmAscent - gm.gmptGlyphOrigin.y;
    INT iBmp_w = gm.gmBlackBoxX + (4 - (gm.gmBlackBoxX % 4)) % 4;
    INT iBmp_h = gm.gmBlackBoxY;
    INT Level = 17;
    INT x, y;
    DWORD Alpha, Color;
    memset(pBits, 0, mappedSubrsrc.RowPitch * tm.tmHeight);
    for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
    {
        for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
        {
            Alpha = (255 * pMono[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
            Color = 0x00ffffff | (Alpha << 24);
            memcpy(static_cast<BYTE*>(pBits) + mappedSubrsrc.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
        }
    }
    DirectGraphics::GetInstance().GetContext()->Unmap(m_layerBuffer, 0);
    // フォント情報の書き込み
    // iOfs_x, iOfs_y : 書き出し位置(左上)
    // iBmp_w, iBmp_h : フォントビットマップの幅高
    // Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)

    // メモリ解放
    delete[] pMono;

    // シェーダリソースビューの設定
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = rtDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = rtDesc.MipLevels;

    // シェーダリソースビューの設定
    if (FAILED(DirectGraphics::GetInstance().GetDevice()->CreateShaderResourceView(m_layerBuffer, &srvDesc, &m_shaderResourceView))) {
        return;
    }

    // Texture設定
    Texture* texture = m_parent->AddComponentIf<Texture>();
    texture->Load(m_shaderResourceView);

    RectTransform *transform = m_parent->GetComponent<RectTransform>();
    transform->m_name        = "Font";
    transform->m_tagName     = "Font";

    transform->m_position.x = -450.0f + 128.0f;
    transform->m_position.y = 224.0f;
    transform->m_scale.x    = 128.0f;
    transform->m_scale.y    = 128.0f;
    
    //transform->m_uv        = XMFLOAT2(1.0f / iOfs_x, 1.0f / iOfs_y);
    //transform->m_frameSize = XMFLOAT2(1.0f / iBmp_w, 1.0f / iBmp_h);
}

void Font::Update() {

}

void Font::Draw() {}