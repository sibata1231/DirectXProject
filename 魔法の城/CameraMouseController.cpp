#include "CameraMouseController.h"
#include "Input.h"

const DirectX::XMFLOAT3 CameraInitPos(0, 0, -5.0f);
const DirectX::XMFLOAT3 CameraInitLook(0, 0, 0);
void CameraMouseController::Init() {
    Camera::Init();
}

void CameraMouseController::Uninit() {
    Camera::Uninit();
}

void CameraMouseController::Update() {
    POINT mousePos = *THE_INPUT.GetMousePosition();
    POINT mouseDist = {
        mousePos.x - m_oldMousePos.x,
        mousePos.y - m_oldMousePos.y,
    };
    DirectX::XMVECTOR vPos = DirectX::XMVectorSet(m_eyePosition.x, m_eyePosition.y, m_eyePosition.z, 0);
    DirectX::XMVECTOR vLook = DirectX::XMVectorSet(m_lookPosition.x, m_lookPosition.y, m_lookPosition.z, 0);
    DirectX::XMVECTOR vFront = DirectX::XMVectorSubtract(vLook, vPos);
    DirectX::XMVECTOR vUp = DirectX::XMVectorSet(m_upVector.x, m_upVector.y, m_upVector.z, 0);
    DirectX::XMVECTOR vSide;
    float focus = 0.0f;

    DirectX::XMStoreFloat(&focus, DirectX::XMVector3Length(vFront));
    vFront = DirectX::XMVector3Normalize(vFront);
    vUp = DirectX::XMVector3Normalize(vUp);
    vSide = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vFront));
    vUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vFront, vSide));

    // 左ボタンダブルクリック(初期化
    /*if (IsMouseDoubleClick(VK_LBUTTON))
    {
        m_pos = CameraInitPos;
        m_look = CameraInitLook;
    }
    else */if (mouseDist.x != 0 || mouseDist.y != 0)
    {
        // 左ボタン(カメラ回り込み
        if (THE_INPUT.GetKeyPress(VK_LBUTTON)) {
            // 回転量
            float angleX = 360.f * mouseDist.x / SCREEN_WIDTH;	// 画面一周で360度回転(画面サイズの半分で180度回転)
            float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT;	// 画面一周で180度回転(画面サイズの半分で90度回転)

            // 回転量をもとに、各軸の回転行列を計算
            DirectX::XMVECTOR rotPos = DirectX::XMVectorSubtract(vPos, vLook);
            DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angleX));
            DirectX::XMVECTOR rotAxis = DirectX::XMVector3TransformCoord(vSide, rotY);
            DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(angleY));

            // 注視点を原点として回転
            rotPos = DirectX::XMVector3TransformCoord(rotPos, rotY);
            rotPos = DirectX::XMVector3TransformCoord(rotPos, rotX);
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(rotPos, vLook));
        }
        // 中ボタン(カメラ移動
        else if (THE_INPUT.GetKeyPress(VK_MBUTTON)) {
            float rate = focus / m_farZ;
            float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
            // 底辺A、高さBとする三角形について tanΘ = A / Bが成り立つ
            // 上記式をもとに割り出した遠景の移動量と、フォーカス位置 / 遠景 の比率から、カメラの移動量を求める
            float farMoveX = tanf(DirectX::XM_PI / 6.0f) * m_farZ * -mouseDist.x * aspect / (SCREEN_WIDTH * 0.5f);
            float farMoveY = tanf(DirectX::XM_PI / 6.0f) * m_farZ *  mouseDist.y / (SCREEN_HEIGHT * 0.5f);
            // 姿勢行列をもとにカメラを移動
            DirectX::XMVECTOR vMove = DirectX::XMVectorAdd(DirectX::XMVectorScale(vSide, farMoveX * rate), DirectX::XMVectorScale(vUp, farMoveY * rate));
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(vPos, vMove));
            DirectX::XMStoreFloat3(&m_lookPosition, DirectX::XMVectorAdd(vLook, vMove));
        }
        // 右ボタン(距離変更
        else if (THE_INPUT.GetKeyPress(VK_RBUTTON)) {
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(vPos, DirectX::XMVectorScale(vFront, focus * mouseDist.y * 0.01f)));
        }
    } else {
        // スクロール
        int wheel = 0;
        if (wheel != 0)
        {
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(vPos, DirectX::XMVectorScale(vFront, focus * wheel * 0.1f)));
        }
    }

    m_oldMousePos = mousePos;     
    Camera::Update();
}