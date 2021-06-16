#include "light.h"
#include "camera.h"

#define LIGHT_DIRECTION	Vector3(2.f,1.0f,2.f)
#define LIGHT_POSITION	Vector3(0.0f,0.0f,0.0f)

void Light::Init() {
	Vector3 vecDir;

	m_diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_position = LIGHT_POSITION;
	vecDir     = LIGHT_DIRECTION;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
}

void Light::Uninit() {
}

void Light::Update() {

}

void Light::SetEnable(bool bEnable) {
	if (bEnable) {
		Vector3 vecDir = LIGHT_DIRECTION;
		XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	} else {
		m_direction = Vector3(0, 0, 0);
	}
}
