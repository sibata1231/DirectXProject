#pragma once
#include "Camera.h"

class CameraMouseController : public Camera {
private:
    POINT m_oldMousePos;
public:
    virtual void Init();
    virtual void Uninit();
    virtual void Update();
};

