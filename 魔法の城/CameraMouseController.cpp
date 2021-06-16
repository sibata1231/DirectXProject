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

    // ���{�^���_�u���N���b�N(������
    /*if (IsMouseDoubleClick(VK_LBUTTON))
    {
        m_pos = CameraInitPos;
        m_look = CameraInitLook;
    }
    else */if (mouseDist.x != 0 || mouseDist.y != 0)
    {
        // ���{�^��(�J������荞��
        if (THE_INPUT.GetKeyPress(VK_LBUTTON)) {
            // ��]��
            float angleX = 360.f * mouseDist.x / SCREEN_WIDTH;	// ��ʈ����360�x��](��ʃT�C�Y�̔�����180�x��])
            float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT;	// ��ʈ����180�x��](��ʃT�C�Y�̔�����90�x��])

            // ��]�ʂ����ƂɁA�e���̉�]�s����v�Z
            DirectX::XMVECTOR rotPos = DirectX::XMVectorSubtract(vPos, vLook);
            DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angleX));
            DirectX::XMVECTOR rotAxis = DirectX::XMVector3TransformCoord(vSide, rotY);
            DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(angleY));

            // �����_�����_�Ƃ��ĉ�]
            rotPos = DirectX::XMVector3TransformCoord(rotPos, rotY);
            rotPos = DirectX::XMVector3TransformCoord(rotPos, rotX);
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(rotPos, vLook));
        }
        // ���{�^��(�J�����ړ�
        else if (THE_INPUT.GetKeyPress(VK_MBUTTON)) {
            float rate = focus / m_farZ;
            float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
            // ���A�A����B�Ƃ���O�p�`�ɂ��� tan�� = A / B�����藧��
            // ��L�������ƂɊ���o�������i�̈ړ��ʂƁA�t�H�[�J�X�ʒu / ���i �̔䗦����A�J�����̈ړ��ʂ����߂�
            float farMoveX = tanf(DirectX::XM_PI / 6.0f) * m_farZ * -mouseDist.x * aspect / (SCREEN_WIDTH * 0.5f);
            float farMoveY = tanf(DirectX::XM_PI / 6.0f) * m_farZ *  mouseDist.y / (SCREEN_HEIGHT * 0.5f);
            // �p���s������ƂɃJ�������ړ�
            DirectX::XMVECTOR vMove = DirectX::XMVectorAdd(DirectX::XMVectorScale(vSide, farMoveX * rate), DirectX::XMVectorScale(vUp, farMoveY * rate));
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(vPos, vMove));
            DirectX::XMStoreFloat3(&m_lookPosition, DirectX::XMVectorAdd(vLook, vMove));
        }
        // �E�{�^��(�����ύX
        else if (THE_INPUT.GetKeyPress(VK_RBUTTON)) {
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(vPos, DirectX::XMVectorScale(vFront, focus * mouseDist.y * 0.01f)));
        }
    } else {
        // �X�N���[��
        int wheel = 0;
        if (wheel != 0)
        {
            DirectX::XMStoreFloat3(&m_eyePosition, DirectX::XMVectorAdd(vPos, DirectX::XMVectorScale(vFront, focus * wheel * 0.1f)));
        }
    }

    m_oldMousePos = mousePos;     
    Camera::Update();
}