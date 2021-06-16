#include "RectTransform.h"

RectTransform::RectTransform() {
    m_uv = XMFLOAT2(0.0f, 0.0f);
    m_frameSize = XMFLOAT2(1.0f, 1.0f);
    m_polygonVertexPos = XMFLOAT4X3(
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f);
    m_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_polygonFrontTexFrame = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
    m_polygonBackTexFrame = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
}

void RectTransform::Start() {
    Transform::Start();
    Transform* transform = m_parent->GetComponent<Transform>();
    transform->m_isEnable = false;
}

void RectTransform::Debug() {
    Transform::Debug();
    ImGui::ColorEdit4((std::string("< color >" + m_name).c_str()), (float *)&m_color);
    ImGui::DragFloat2((std::string("< uv    >" + m_name).c_str()), (float *)&m_uv, 0.1f);
    ImGui::DragFloat2((std::string("< frame >" + m_name).c_str()), (float *)&m_frameSize,0.1f);
}