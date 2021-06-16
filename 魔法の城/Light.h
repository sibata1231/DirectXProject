#pragma once
#include "SystemMain.h"
#include "Singleton.h"
#include "GraphicsUtility.h"

class Light : public Singleton<Light> {
public:
    XMFLOAT4 m_diffuse;
    XMFLOAT4 m_ambient;
    XMFLOAT4 m_specular;
    Vector3  m_position;
    Vector3  m_direction;
public:
    void Init(void);
    void Uninit(void);
    void Update(void);
    void SetEnable(bool bEnable);
};