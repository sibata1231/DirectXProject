#include "Rigidbody.h"

#define BOUNCE_COEFFICIENT	(0.8f)

Rigidbody::Rigidbody() {
    m_velocity = Vector3(0.0f, 0.0f, 0.0f);
    m_mass     = 1.0f;
}

void Rigidbody::Update() {
    m_transform->m_position.x += m_velocity.x;
    m_transform->m_position.y += m_velocity.y;
    m_transform->m_position.z += m_velocity.z;
}

void Rigidbody::Bounce(Rigidbody& t1, Rigidbody& t2) {
    float t;
    float vx = (t2.m_transform->m_position.x - t1.m_transform->m_position.x);
    float vy = (t2.m_transform->m_position.y - t1.m_transform->m_position.y);

    t = -(vx * t1.m_velocity.x + vy *  t1.m_velocity.y) / (vx * vx + vy * vy);
    float arx = t1.m_velocity.x + vx * t;
    float ary = t1.m_velocity.y + vy * t;

    t = -(-vy * t1.m_velocity.x + vx * t1.m_velocity.y) / (vy * vy + vx * vx);
    float amx = t1.m_velocity.x - vy * t;
    float amy = t1.m_velocity.y + vx * t;

    t = -(vx *  t2.m_velocity.x + vy * t2.m_velocity.y) / (vx * vx + vy * vy);
    float brx = t2.m_velocity.x + vx * t;
    float bry = t2.m_velocity.y + vy * t;

    t = -(-vy * t2.m_velocity.x + vx * t2.m_velocity.y) / (vy * vy + vx * vx);
    float bmx = t2.m_velocity.x - vy * t;
    float bmy = t2.m_velocity.y + vx * t;

    float e = BOUNCE_COEFFICIENT;
    float am = t1.m_mass;
    float bm = t2.m_mass;
    float adx = (am * amx + bm * bmx + bmx * e * bm - amx * e * bm) / (am + bm);
    float bdx = -e * (bmx - amx) + adx;
    float ady = (am * amy + bm * bmy + bmy * e * bm - amy * e * bm) / (am + bm);
    float bdy = -e * (bmy - amy) + ady;

    t1.m_velocity.x = adx + arx;
    t1.m_velocity.y = ady + ary;
    t2.m_velocity.x = bdx + brx;
    t2.m_velocity.y = bdy + bry;
}