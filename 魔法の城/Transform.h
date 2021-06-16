#ifndef ___TRANSFORM_H___
#define ___TRANSFORM_H___
#include "Object.h"
#include "Corutine.h"

class Transform : public Component {
public:
    Vector3    m_position;
    Vector3    m_rotation;
    Vector3    m_scale;
    XMFLOAT4X4  m_world;
    std::string m_name;
    std::string m_tagName;
    Object*     m_object;
private:
    Vector3 m_criteriaPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_criteriaRotate   = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_criteriaScale    = Vector3(0.0f, 0.0f, 0.0f);
    bool    m_isMoveTween;
    bool    m_isRotateTween;
    bool    m_isScaleTween;
public:
    Transform();
    virtual ~Transform()  {};
    virtual void Start()  {};
    virtual void Draw()   {};
    virtual void Update() {};
    virtual void Debug();
    std::string GetName() { 
        return m_name; 
    }

    std::string GetTagName() {
        return m_tagName;
    }

    void SetWorld(XMFLOAT4X4& world) {
        m_world = world;
    }

    XMFLOAT4X4 GetWorld() {
        return m_world;
    }

    Vector3 GetPos() {
        return m_position;
    }

    void SetPos(Vector3& pos) {
        m_position.x = m_world._41 = pos.x;
        m_position.y = m_world._42 = pos.y;
        m_position.z = m_world._43 = pos.z;
    }

    void SetScale(float& scale) {
        m_scale = Vector3(scale, scale, scale);
        m_world._11 *= scale;
        m_world._22 *= scale;
        m_world._33 *= scale;
    }

    void SetScale(Vector3& scale) {
        m_scale = scale;
        m_world._11 *= scale.x;
        m_world._22 *= scale.y;
        m_world._33 *= scale.z;
    }

    void UpdateMatrix() {
        XMMATRIX mtxWorld, mtxScale, mtxRot, mtxTranslate;
        mtxWorld = XMMatrixIdentity();
        mtxScale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
        mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
        mtxRot = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y + XM_PI, m_rotation.z);
        mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
        mtxTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
        mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
        XMStoreFloat4x4(&m_world, mtxWorld);
    }

    // DoTween—p
private:
    std::list<std::function<void(Transform)>> m_completedList;
    std::future<int> DOMove(float move, float time, int type = 0);
    std::future<int> DORotate(float move, float time,  int type = 0);
    std::future<int> DOScale(float move, float time, int type = 0);
    void Completed();
public:
    std::future<int> DOMove(Vector3 move, float time);
    std::future<int> DORotate(Vector3 move, float time);
    std::future<int> DOScale(Vector3 move, float time);
    void ResetTween() {
        m_criteriaPosition = Vector3(0.0f, 0.0f, 0.0f);
        m_criteriaRotate   = Vector3(0.0f, 0.0f, 0.0f);
        m_criteriaScale    = Vector3(0.0f, 0.0f, 0.0f);
    }
    Transform& DOMoveX(float value, float time) {
        if (!m_isMoveTween) {
            DOMove(value, time, 0);
        }
        return *this;
    }
    Transform& DOMoveY(float value, float time, const bool& isDoPlay = false) {
        if (!m_isMoveTween) {
            DOMove(value, time, 1);
        }
        return *this;
    }
    Transform& DOMoveZ(float value, float time, const bool& isDoPlay = false) {
        if (!m_isMoveTween) {
            DOMove(value, time, 2);
        }
        return *this;
    }
    Transform& DORotateX(float value, float time) {
        if (!m_isRotateTween) {
            DORotate(value, time, 0);
        }
        return *this;
    }
    Transform& DORotateY(float value, float time, const bool& isDoPlay = false) {
        if (!m_isRotateTween) {
            DORotate(value, time, 1);
        }
        return *this;
    }
    Transform& DORotateZ(float value, float time, const bool& isDoPlay = false) {
        if (!m_isRotateTween) {
            DORotate(value, time, 2);
        }
        return *this;
    }
    Transform& DOScaleX(float value, float time) {
        if (!m_isScaleTween) {
            DOScale(value, time, 0);
        }
        return *this;
    }
    Transform& DOScaleY(float value, float time, const bool& isDoPlay = false) {
        if (!m_isScaleTween) {
            DOScale(value, time, 1);
        }
        return *this;
    }
    Transform& DOScaleZ(float value, float time, const bool& isDoPlay = false) {
        if (!m_isScaleTween) {
            DOScale(value, time, 2);
        }
        return *this;
    }

    Transform& OnComplete(std::function<void(Transform)> completed) {
        m_completedList.push_back(completed);
        return *this;
    }

    const bool IsPlay() {
        return m_isMoveTween && m_isRotateTween && m_isScaleTween;
    }
    Transform& SetRelative() {
        if (!IsPlay()) {
            return *this;
        }
        m_criteriaPosition = m_position;
        m_criteriaRotate   = m_rotation;
        m_criteriaScale    = m_scale;
        return *this;
    }
};

#endif