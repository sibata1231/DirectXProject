#include "SampleObject.h"
#include "Camera.h"
#include "Light.h"
#include "Player.h"
#include "input.h"

#define PATH_SAMPLE_OBJECT ("data/model/Dragon.fbx") // モデル
#define SPEED              (15.0f)                   // 速さ

// 初期化
void SampleObject::Start() {
    Model *model =  m_parent->AddComponentIf<Model>();
    model->Create("SampleObject", PATH_SAMPLE_OBJECT);
    m_transform->m_name          = "SampleObject";
    m_transform->m_tagName       = "SampleObject";
}

// 更新
void SampleObject::Update() {
    // モデル座標軸取得
    Vector3 vX = Vector3(m_transform->m_world._11, m_transform->m_world._12, m_transform->m_world._13);
    Vector3 vY = Vector3(m_transform->m_world._21, m_transform->m_world._22, m_transform->m_world._23);
    Vector3 vZ = Vector3(m_transform->m_world._31, m_transform->m_world._32, m_transform->m_world._33);

    // 座標の取得とクリア
    Vector3 vP = { m_transform->m_world._41,m_transform->m_world._42 ,m_transform->m_world._43 };
            
    XMVECTOR pout;
    Vector3 point;
    CalcScreenToXZ(&pout, THE_INPUT.GetMousePosition()->x, THE_INPUT.GetMousePosition()->y, SCREEN_WIDTH, SCREEN_HEIGHT, &XMLoadFloat4x4(&MainCamera::GetInstance().GetView()), &XMLoadFloat4x4(&MainCamera::GetInstance().GetProjection()));
    point = pout;
    vP = point;

    // 座標をワールド行列に反映
    m_transform->m_position.x = m_transform->m_world._41 = vP.x;
    m_transform->m_position.y = m_transform->m_world._42 = vP.y;
    m_transform->m_position.z = m_transform->m_world._43 = vP.z;
}

// 描画
void SampleObject::Draw() {

}

// スクリーン座標をワールド座標に変換（ CalcScreenToWorld関数 ）
// スクリーン座標をワールド座標に変換
XMVECTOR* SampleObject::CalcScreenToWorld(
    XMVECTOR* pout,
    int Sx,  // スクリーンX座標
    int Sy,  // スクリーンY座標
    float fZ,  // 射影空間でのZ値（0～1）
    int Screen_w,
    int Screen_h,
    XMMATRIX* View,
    XMMATRIX* Prj) {
    // 各行列の逆行列を算出
    XMMATRIX InvView, InvPrj, InvViewport;
    XMFLOAT4X4 VP;
    InvView = XMMatrixInverse(nullptr, *View);
    InvPrj  = XMMatrixInverse(nullptr, *Prj);
    XMStoreFloat4x4(&VP, XMMatrixIdentity());
    VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
    VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
    InvViewport = XMMatrixInverse(nullptr, XMLoadFloat4x4(&VP));
    
    // 逆変換
    XMMATRIX tmp = InvViewport * InvPrj * InvView;
    *pout = XMVector3TransformCoord(XMVectorSet(static_cast<float>(Sx), static_cast<float>(Sy), fZ, 1.0f), tmp);
    return pout;
}

// XZ平面とスクリーン座標の交点算出関数
XMVECTOR* SampleObject::CalcScreenToXZ(
    XMVECTOR* pout,
    int Sx,
    int Sy,
    int Screen_w,
    int Screen_h,
    XMMATRIX* View,
    XMMATRIX* Prj){
    XMVECTOR nearpos;
    XMVECTOR farpos;
    XMVECTOR ray;
    CalcScreenToWorld(&nearpos, Sx, Sy, 0.0f, Screen_w, Screen_h, View, Prj);
    CalcScreenToWorld(&farpos, Sx, Sy, 1.0f, Screen_w, Screen_h, View, Prj);
    ray = farpos - nearpos;
    ray = XMVector3Normalize(ray);
    // 床との交差が起きている場合は交点を
    // 起きていない場合は遠くの壁との交点を出力
    if (XMVectorGetY(ray) <= 0) {
        // 床交点
        float Lray = static_cast<Vector3>(XMVector3Length(XMVector3Dot(ray, XMVectorSet(0, 1, 0, 1.0f)))).x;
        float LP0  = static_cast<Vector3>(XMVector3Length(XMVector3Dot((-nearpos), XMVectorSet(0, 1, 0, 1.0f)))).x;
        *pout      = nearpos + (LP0 / Lray) * ray;
    } else {
        *pout = farpos;
    }
    return pout;
}