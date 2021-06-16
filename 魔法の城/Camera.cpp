#include "Camera.h"
#include "CameraMouseController.h"
//#include "GMCameraController.h"

// 初期化
void Camera::Init() {
    //m_eyePosition  = Vector3(0.0f, 45.0f, 300.0f);        // 視点座標
    //m_lookPosition = Vector3(0.0f, 20.0f, 0.0f);          // 注視点座標
    m_eyePosition  = Vector3(0.0f, 400.0f, 200.0f);        // 視点座標
    m_lookPosition = Vector3(0.0f, 20.0f, 0.0f);          // 注視点座標
    m_upVector     = Vector3(0.0f, 1.0f, 0.0f);           // 上方ベクトル
    m_fovy         = XMConvertToRadians(30);              // 視野角
    m_nearZ        = 10.0f;                               // 前方クリップ距離
    m_farZ         = 1000.0f;                             // 後方クリップ距離
    m_aspext       = (float)1024 / 576; // アスペクト
    m_nowEye       = m_eyePosition;                       // 現在の視点
    m_nowLook      = m_lookPosition;                      // 現在の注視点
    m_nowUp        = m_upVector;                          // 現在の上方ベクトル
    XMStoreFloat3(&m_nowUp, XMVector3Normalize(XMLoadFloat3(&m_nowUp)));
    XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(XMLoadFloat3(&m_nowEye), XMLoadFloat3(&m_nowLook), XMLoadFloat3(&m_nowUp)));
    XMStoreFloat4x4(&m_projection, XMMatrixPerspectiveFovLH(m_fovy, m_aspext, m_nearZ, m_farZ));
}

void Camera::Uninit() {

}

void Camera::Update() {
    //m_nowEye.x  = m_nowEye.x  * 0.5f + m_eyePosition.x  * 0.5f;
    //m_nowEye.y  = m_nowEye.y  * 0.5f + m_eyePosition.y  * 0.5f;
    //m_nowEye.z  = m_nowEye.z  * 0.5f + m_eyePosition.z  * 0.5f;
    //m_nowLook.x = m_nowLook.x * 0.5f + m_lookPosition.x * 0.5f;
    //m_nowLook.y = m_nowLook.y * 0.5f + m_lookPosition.y * 0.5f;
    //m_nowLook.z = m_nowLook.z * 0.5f + m_lookPosition.z * 0.5f;
    //m_nowUp.x   = m_nowUp.x   * 0.5f + m_upVector.x     * 0.5f;
    //m_nowUp.y   = m_nowUp.y   * 0.5f + m_upVector.y     * 0.5f;
    //m_nowUp.z   = m_nowUp.z   * 0.5f + m_upVector.z     * 0.5f;
    m_nowEye  = m_eyePosition;
    m_nowLook = m_lookPosition;
    m_nowUp   = m_upVector;

    XMStoreFloat3(&m_nowUp, XMVector3Normalize(XMLoadFloat3(&m_nowUp)));
    XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(XMLoadFloat3(&m_nowEye), XMLoadFloat3(&m_nowLook), XMLoadFloat3(&m_nowUp)));
    XMStoreFloat4x4(&m_projection, XMMatrixPerspectiveFovLH(m_fovy, m_aspext, m_nearZ, m_farZ));
}

void MainCamera::Init() {
    m_indexCount = 0;
    Add(new CameraMouseController());
    SetTarget(0);
    m_isController = false;
}

void MainCamera::Uninit() {
    for (int i = 0; i < m_indexCount; i++) {
        SAFE_DELETE(m_cameraIndex[i]);
    }
}

void MainCamera::Update() {
    if (!m_isController) {
        return;
    }
    m_mainCamera->Update();
}

void MainCamera::Add(Camera *camera) {
    m_cameraIndex[m_indexCount] = camera; 
    m_indexCount++; 
}

void MainCamera::SetTarget(int index) {
    if (index < 0 || m_indexCount <= index) {
        return;
    }
    m_mainCamera = nullptr;
    m_mainCamera = m_cameraIndex[index];
    m_mainCamera->Init();
}