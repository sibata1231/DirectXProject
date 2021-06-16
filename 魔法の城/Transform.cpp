#include "Transform.h"

Transform::Transform() {
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());
    m_position = Vector3(0.0f, 0.0f, 0.0f);
    m_rotation = Vector3(0.0f, 0.0f, 0.0f);
    m_scale    = Vector3(1.0f, 1.0f, 1.0f);
    m_name     = "noname";
    m_tagName  = "defalut";
    m_isMoveTween = false;
    m_isRotateTween = false;
    m_isScaleTween = false;
}

void Transform::Debug() {
    ImGui::Text((std::string("[ Name ] : ") + m_name).c_str());
    ImGui::SameLine();
    ImGui::Text((std::string("[ Tag ] : ") + m_tagName).c_str());
    ImGui::DragFloat3((std::string("< position >" + m_name).c_str()), (float *)&m_position);
    ImGui::DragFloat3((std::string("< rotation >" + m_name).c_str()), (float *)&m_rotation);
    ImGui::DragFloat3((std::string("< scale    >" + m_name).c_str()), (float *)&m_scale, 0.01f);
    ImGui::Text("%d", m_isMoveTween);
}

std::future<int> Transform::DOMove(Vector3 value, float time) {
    int i          = 0;
    float count    = 100 * time;
    m_isMoveTween  = true;
    for (i = 0; i <= count; i++) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        m_position.x = m_criteriaPosition.x + value.x / count * i;
        m_position.y = m_criteriaPosition.y + value.y / count * i;
        m_position.z = m_criteriaPosition.z + value.z / count * i;
    }
    m_criteriaPosition = m_position;
    m_isMoveTween = false;
    Completed();
    return i;
}

std::future<int> Transform::DOMove(float move, float time, int type) {
    int i         = 0;
    float count   = 100 * time;
    m_isMoveTween = true;
    for (i = 0; i <= count; i++) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        switch (type) {
            case 0: m_position.x = m_criteriaPosition.x + move / count * i; break;
            case 1: m_position.y = m_criteriaPosition.y + move / count * i; break;
            case 2: m_position.z = m_criteriaPosition.z + move / count * i; break;
        }
    }
    m_criteriaPosition = m_position;
    m_isMoveTween = false;
    Completed();
    return i;
}

std::future<int> Transform::DORotate(Vector3 value, float time) {
    int i = 0;
    float count = 100 * time;
    m_isRotateTween = true;
    for (i = 0; i <= count; i++) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        m_rotation.x = m_criteriaRotate.x + value.x / count * i;
        m_rotation.y = m_criteriaRotate.y + value.y / count * i;
        m_rotation.z = m_criteriaRotate.z + value.z / count * i;
    }
    //m_criteriaRotate = m_rotation;
    m_isRotateTween = false;
    Completed();
    return i;
}

std::future<int> Transform::DORotate(float rotate, float time, int type) {
    int i           = 0;
    float count     = 100 * time;
    m_isRotateTween = true;
    for (i = 0; i <= count; i++) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        switch (type) {
            case 0: m_rotation.x = m_criteriaRotate.x + rotate / count * i; break;
            case 1: m_rotation.y = m_criteriaRotate.y + rotate / count * i; break;
            case 2: m_rotation.z = m_criteriaRotate.z + rotate / count * i; break;
        }
    }
    m_criteriaRotate = m_rotation;
    m_isRotateTween = false;
    Completed();
    return i;
}

std::future<int> Transform::DOScale(Vector3 value, float time) {
    int i = 0;
    float count = 100 * time;
    m_isScaleTween = true;
    for (i = 0; i <= count; i++) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        m_scale.x = m_criteriaScale.x + value.x / count * i;
        m_scale.y = m_criteriaScale.y + value.y / count * i;
        m_scale.z = m_criteriaScale.z + value.z / count * i;
    }
    m_criteriaScale = m_scale;
    m_isScaleTween = false;
    Completed();
    return i;
}

std::future<int> Transform::DOScale(float scale, float time, int type) {
    int i          = 0;
    float count    = 100 * time;
    m_isScaleTween = true;
    m_scale        = m_criteriaScale;
    for (i = 0; i <= count; i++) {
        co_await std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        switch (type) {
            case 0: m_scale.x = m_criteriaScale.x + scale / count * i; break;
            case 1: m_scale.y = m_criteriaScale.y + scale / count * i; break;
            case 2: m_scale.z = m_criteriaScale.z + scale / count * i; break;
        }
    }
    m_criteriaScale = m_scale;
    m_isScaleTween = false;
    Completed();
    return i;
}

void Transform::Completed() {
    if (m_completedList.size() > 0) {
        for (auto itr = m_completedList.begin(); itr != m_completedList.end(); itr++) {
            auto completedFunction = *itr;
            completedFunction(*this);
        }
        m_completedList.clear();
    }
}