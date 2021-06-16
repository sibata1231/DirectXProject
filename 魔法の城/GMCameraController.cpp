#include "GMCameraController.h"
#include "Input.h"
#define	VIEW_ANGLE			(XMConvertToRadians(45.0f))					// ビュー平面の視野角
#define	VIEW_ASPECT			((float)1024/(float)576)	// ビュー平面のアスペクト比
#define	VIEW_NEAR_Z			(10.0f)										// ビュー平面のNearZ値
#define	VIEW_FAR_Z			(30000.0f)									// ビュー平面のFarZ値
#define	VALUE_MOVE_CAMERA	(2.0f)										// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI*0.01f)								// カメラの回転量

#define	VALUE_MOVE_DRAG		(0.30f)
#define	VALUE_ROTATE_MOUSE	(0.003f)


void GMCameraController::Init() {
    float fLengthXZ;
    
    m_eyePosition = Vector3(0.0f, 200.0f, -700.0f);
    m_lookPosition = Vector3(0.0f, 50.0f, 0.0f);
    m_upVector = Vector3(0.0f, 1.0f, 0.0f);
    m_rotation = Vector3(0.0f, 0.0f, 0.0f);
    m_aspext = VIEW_ASPECT;
    m_nearZ = VIEW_NEAR_Z;
    m_farZ = VIEW_FAR_Z;
    m_fovy = XMConvertToRadians(VIEW_ANGLE);

    m_lengthInterval = sqrtf((m_eyePosition.x - m_lookPosition.x) * (m_eyePosition.x - m_lookPosition.x)
        + (m_eyePosition.y - m_lookPosition.y) * (m_eyePosition.y - m_lookPosition.y)
        + (m_eyePosition.z - m_lookPosition.z) * (m_eyePosition.z - m_lookPosition.z));
    fLengthXZ = sqrtf((m_eyePosition.x - m_lookPosition.x) * (m_eyePosition.x - m_lookPosition.x)
        + (m_eyePosition.z - m_lookPosition.z) * (m_eyePosition.z - m_lookPosition.z));
    m_rotation.y = atan2f((m_lookPosition.x - m_eyePosition.x), (m_lookPosition.z - m_eyePosition.z));
    m_rotation.x = atan2f((m_lookPosition.y - m_eyePosition.y), fLengthXZ);

    m_lengthIntervalXZ = cosf(m_rotation.x) * m_lengthInterval;

    m_mousePosition = *Input::GetInstance().GetMousePosition();

    m_nowEye = m_eyePosition;                       // 現在の視点
    m_nowLook = m_lookPosition;                      // 現在の注視点
    m_nowUp = m_upVector;                          // 現在の上方ベクトル
    XMStoreFloat3(&m_nowUp, XMVector3Normalize(XMLoadFloat3(&m_nowUp)));
    XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(XMLoadFloat3(&m_nowEye), XMLoadFloat3(&m_nowLook), XMLoadFloat3(&m_nowUp)));
    XMStoreFloat4x4(&m_projection, XMMatrixPerspectiveFovLH(m_fovy, m_aspext, m_nearZ, m_farZ));
}

void GMCameraController::Uninit() {
    Camera::Uninit();
}

void GMCameraController::Update() {
    float valueX, valueY;
    POINT ptMouse;

    // マウス取得
    ptMouse = *Input::GetInstance().GetMousePosition();
    valueX = (float)(ptMouse.x - m_mousePosition.x);
    valueY = (float)(ptMouse.y - m_mousePosition.y);
    m_mousePosition = ptMouse;

    if (Input::GetInstance().GetKeyPress(VK_A)) {
        if (Input::GetInstance().GetKeyPress(VK_W)) {
            // 左前移動
            m_eyePosition.x -= cosf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z += sinf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else if (Input::GetInstance().GetKeyPress(VK_S)) {
            // 左後移動
            m_eyePosition.x -= cosf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z += sinf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else {
            // 左移動
            m_eyePosition.x -= cosf(m_rotation.y) * VALUE_MOVE_CAMERA;
            m_eyePosition.z += sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    else if (Input::GetInstance().GetKeyPress(VK_D)) {
        if (Input::GetInstance().GetKeyPress(VK_W)) {
            // 右前移動
            m_eyePosition.x += cosf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z -= sinf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else if (Input::GetInstance().GetKeyPress(VK_S)) {
            // 右後移動
            m_eyePosition.x += cosf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z -= sinf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else {
            // 右移動
            m_eyePosition.x += cosf(m_rotation.y) * VALUE_MOVE_CAMERA;
            m_eyePosition.z -= sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    else if (Input::GetInstance().GetKeyPress(VK_W)) {
        // 前移動
        m_eyePosition.x += sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        m_eyePosition.z += cosf(m_rotation.y) * VALUE_MOVE_CAMERA;

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    else if (Input::GetInstance().GetKeyPress(VK_S)) {
        // 後移動
        m_eyePosition.x -= sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        m_eyePosition.z -= cosf(m_rotation.y) * VALUE_MOVE_CAMERA;

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }

    if (Input::GetInstance().GetKeyPress(VK_Q)) {
        // 視点旋回「左」
        m_rotation.y += VALUE_ROTATE_CAMERA;
        if (m_rotation.y > XM_PI) {
            m_rotation.y -= XM_2PI;
        }

        m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_E)) {
        // 視点旋回「右」
        m_rotation.y -= VALUE_ROTATE_CAMERA;
        if (m_rotation.y < -XM_PI) {
            m_rotation.y += XM_2PI;
        }

        m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_Y)) {
        // 視点移動「上」
        m_eyePosition.y += VALUE_MOVE_CAMERA;
    }
    if (Input::GetInstance().GetKeyPress(VK_N)) {
        // 視点移動「下」
        m_eyePosition.y -= VALUE_MOVE_CAMERA;
    }

    if (Input::GetInstance().GetKeyPress(VK_U)) {
        // 注視点旋回「左」
        m_rotation.y -= VALUE_ROTATE_CAMERA;
        if (m_rotation.y < -XM_PI) {
            m_rotation.y += XM_2PI;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_O)) {
        // 注視点旋回「右」
        m_rotation.y += VALUE_ROTATE_CAMERA;
        if (m_rotation.y > XM_PI) {
            m_rotation.y -= XM_2PI;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_T)) {
        // 注視点移動「上」
        m_lookPosition.y += VALUE_MOVE_CAMERA;
    }
    if (Input::GetInstance().GetKeyPress(VK_B)) {
        // 注視点移動「下」
        m_lookPosition.y -= VALUE_MOVE_CAMERA;
    }

    if (Input::GetInstance().GetMouseButton(MOUSEBUTTON_L)) {
        if (Input::GetInstance().GetMouseButton(MOUSEBUTTON_R)) {
            // 移動
            if (valueX != 0 || valueY != 0) {
                float angle = atan2f(valueX, valueY);
                float length = sqrtf(valueX*valueX + valueY * valueY) * VALUE_MOVE_DRAG;

                angle -= m_rotation.y;
                if (angle < -XM_PI) angle += XM_2PI;

                m_lookPosition.x -= sinf(angle) * length;
                m_lookPosition.z += cosf(angle) * length;

                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }
        }
        else {
            // 回転
            if (valueX < 0) {
                // 視点移動「左」
                m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
                if (m_rotation.y < -XM_PI) {
                    m_rotation.y += XM_2PI;
                }

                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }
            else if (valueX > 0) {
                // 視点移動「右」
                m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
                if (m_rotation.y > XM_PI) {
                    m_rotation.y -= XM_2PI;
                }

                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }

            if (valueY < 0) {
                // 視点移動「上」
                m_rotation.x -= XM_PI * valueY * VALUE_ROTATE_MOUSE;
                if (m_rotation.x > (XM_PIDIV2 - XM_PI * 0.02f)) {
                    m_rotation.x = (XM_PIDIV2 - XM_PI * 0.02f);
                }

                m_eyePosition.y = m_lookPosition.y - sinf(m_rotation.x) * m_lengthInterval;

                m_lengthIntervalXZ = cosf(m_rotation.x) * m_lengthInterval;
                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }
            else if (valueY > 0) {
                // 視点移動「下」
                m_rotation.x -= XM_PI * valueY * VALUE_ROTATE_MOUSE;
                if (m_rotation.x < (-XM_PIDIV2 + XM_PI * 0.02f)) {
                    m_rotation.x = (-XM_PIDIV2 + XM_PI * 0.02f);
                }

                m_eyePosition.y = m_lookPosition.y - sinf(m_rotation.x) * m_lengthInterval;

                m_lengthIntervalXZ = cosf(m_rotation.x) * m_lengthInterval;
                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }
        }
    }
    else if (Input::GetInstance().GetMouseButton(MOUSEBUTTON_R)) {
        if (valueX < 0) {
            // 注視点移動「左」
            m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
            if (m_rotation.y < -XM_PI) {
                m_rotation.y += XM_2PI;
            }

            m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
            m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
        }
        else if (valueX > 0) {
            // 注視点移動「右」
            m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
            if (m_rotation.y > XM_PI) {
                m_rotation.y -= XM_2PI;
            }

            m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
            m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
        }

        if (valueY < 0) {
            // 注視点移動「上」
            m_rotation.x -= XM_PI * valueY * VALUE_ROTATE_MOUSE;
            if (m_rotation.x > (XM_PIDIV2 - XM_PI * 0.1f)) {
                m_rotation.x = (XM_PIDIV2 - XM_PI * 0.1f);
            }

            m_lookPosition.y = m_eyePosition.y + sinf(m_rotation.x) * m_lengthInterval;

            m_lengthIntervalXZ = cosf(m_rotation.x) * m_lengthInterval;
            m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
            m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
        }
        else if (valueY > 0) {
            // 注視点移動「下」
            m_rotation.x -= XM_PI * valueY * VALUE_ROTATE_MOUSE;
            if (m_rotation.x < (-XM_PIDIV2 + XM_PI * 0.1f)) {
                m_rotation.x = (-XM_PIDIV2 + XM_PI * 0.1f);
            }

            m_lookPosition.y = m_eyePosition.y + sinf(m_rotation.x) * m_lengthInterval;

            m_lengthIntervalXZ = cosf(m_rotation.x) * m_lengthInterval;
            m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
            m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
        }
    }
    Camera::Update();
}