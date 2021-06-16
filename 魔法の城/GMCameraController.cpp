#include "GMCameraController.h"
#include "Input.h"
#define	VIEW_ANGLE			(XMConvertToRadians(45.0f))					// �r���[���ʂ̎���p
#define	VIEW_ASPECT			((float)1024/(float)576)	// �r���[���ʂ̃A�X�y�N�g��
#define	VIEW_NEAR_Z			(10.0f)										// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z			(30000.0f)									// �r���[���ʂ�FarZ�l
#define	VALUE_MOVE_CAMERA	(2.0f)										// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI*0.01f)								// �J�����̉�]��

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

    m_nowEye = m_eyePosition;                       // ���݂̎��_
    m_nowLook = m_lookPosition;                      // ���݂̒����_
    m_nowUp = m_upVector;                          // ���݂̏���x�N�g��
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

    // �}�E�X�擾
    ptMouse = *Input::GetInstance().GetMousePosition();
    valueX = (float)(ptMouse.x - m_mousePosition.x);
    valueY = (float)(ptMouse.y - m_mousePosition.y);
    m_mousePosition = ptMouse;

    if (Input::GetInstance().GetKeyPress(VK_A)) {
        if (Input::GetInstance().GetKeyPress(VK_W)) {
            // ���O�ړ�
            m_eyePosition.x -= cosf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z += sinf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else if (Input::GetInstance().GetKeyPress(VK_S)) {
            // ����ړ�
            m_eyePosition.x -= cosf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z += sinf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else {
            // ���ړ�
            m_eyePosition.x -= cosf(m_rotation.y) * VALUE_MOVE_CAMERA;
            m_eyePosition.z += sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    else if (Input::GetInstance().GetKeyPress(VK_D)) {
        if (Input::GetInstance().GetKeyPress(VK_W)) {
            // �E�O�ړ�
            m_eyePosition.x += cosf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z -= sinf(m_rotation.y - XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else if (Input::GetInstance().GetKeyPress(VK_S)) {
            // �E��ړ�
            m_eyePosition.x += cosf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
            m_eyePosition.z -= sinf(m_rotation.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
        }
        else {
            // �E�ړ�
            m_eyePosition.x += cosf(m_rotation.y) * VALUE_MOVE_CAMERA;
            m_eyePosition.z -= sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    else if (Input::GetInstance().GetKeyPress(VK_W)) {
        // �O�ړ�
        m_eyePosition.x += sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        m_eyePosition.z += cosf(m_rotation.y) * VALUE_MOVE_CAMERA;

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    else if (Input::GetInstance().GetKeyPress(VK_S)) {
        // ��ړ�
        m_eyePosition.x -= sinf(m_rotation.y) * VALUE_MOVE_CAMERA;
        m_eyePosition.z -= cosf(m_rotation.y) * VALUE_MOVE_CAMERA;

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }

    if (Input::GetInstance().GetKeyPress(VK_Q)) {
        // ���_����u���v
        m_rotation.y += VALUE_ROTATE_CAMERA;
        if (m_rotation.y > XM_PI) {
            m_rotation.y -= XM_2PI;
        }

        m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_E)) {
        // ���_����u�E�v
        m_rotation.y -= VALUE_ROTATE_CAMERA;
        if (m_rotation.y < -XM_PI) {
            m_rotation.y += XM_2PI;
        }

        m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_Y)) {
        // ���_�ړ��u��v
        m_eyePosition.y += VALUE_MOVE_CAMERA;
    }
    if (Input::GetInstance().GetKeyPress(VK_N)) {
        // ���_�ړ��u���v
        m_eyePosition.y -= VALUE_MOVE_CAMERA;
    }

    if (Input::GetInstance().GetKeyPress(VK_U)) {
        // �����_����u���v
        m_rotation.y -= VALUE_ROTATE_CAMERA;
        if (m_rotation.y < -XM_PI) {
            m_rotation.y += XM_2PI;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_O)) {
        // �����_����u�E�v
        m_rotation.y += VALUE_ROTATE_CAMERA;
        if (m_rotation.y > XM_PI) {
            m_rotation.y -= XM_2PI;
        }

        m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
        m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
    }
    if (Input::GetInstance().GetKeyPress(VK_T)) {
        // �����_�ړ��u��v
        m_lookPosition.y += VALUE_MOVE_CAMERA;
    }
    if (Input::GetInstance().GetKeyPress(VK_B)) {
        // �����_�ړ��u���v
        m_lookPosition.y -= VALUE_MOVE_CAMERA;
    }

    if (Input::GetInstance().GetMouseButton(MOUSEBUTTON_L)) {
        if (Input::GetInstance().GetMouseButton(MOUSEBUTTON_R)) {
            // �ړ�
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
            // ��]
            if (valueX < 0) {
                // ���_�ړ��u���v
                m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
                if (m_rotation.y < -XM_PI) {
                    m_rotation.y += XM_2PI;
                }

                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }
            else if (valueX > 0) {
                // ���_�ړ��u�E�v
                m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
                if (m_rotation.y > XM_PI) {
                    m_rotation.y -= XM_2PI;
                }

                m_eyePosition.x = m_lookPosition.x - sinf(m_rotation.y) * m_lengthIntervalXZ;
                m_eyePosition.z = m_lookPosition.z - cosf(m_rotation.y) * m_lengthIntervalXZ;
            }

            if (valueY < 0) {
                // ���_�ړ��u��v
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
                // ���_�ړ��u���v
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
            // �����_�ړ��u���v
            m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
            if (m_rotation.y < -XM_PI) {
                m_rotation.y += XM_2PI;
            }

            m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
            m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
        }
        else if (valueX > 0) {
            // �����_�ړ��u�E�v
            m_rotation.y += XM_PI * valueX * VALUE_ROTATE_MOUSE;
            if (m_rotation.y > XM_PI) {
                m_rotation.y -= XM_2PI;
            }

            m_lookPosition.x = m_eyePosition.x + sinf(m_rotation.y) * m_lengthIntervalXZ;
            m_lookPosition.z = m_eyePosition.z + cosf(m_rotation.y) * m_lengthIntervalXZ;
        }

        if (valueY < 0) {
            // �����_�ړ��u��v
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
            // �����_�ړ��u���v
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