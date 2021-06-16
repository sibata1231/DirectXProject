#pragma once
#include "Camera.h"

class GMCameraController : public Camera {
private:
    POINT   m_mousePosition;
    Vector3 m_rotation;
    float   m_lengthInterval;
    float   m_lengthIntervalXZ;
public:
    virtual void Init();
    virtual void Uninit();
    virtual void Update();
};

