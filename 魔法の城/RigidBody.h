#ifndef ___RIGID_BODY_H___
#define ___RIGID_BODY_H___

#include "Transform.h"

struct Friction {
    bool  isActive;
    float power;
};

class Rigidbody : public Component {
public:
    Friction    m_friction;
    float		m_mass;
    Vector3	    m_velocity;
    bool        m_isGravity;
    bool        m_isEnter;
public:
    Rigidbody();
    virtual		~Rigidbody() {};
    virtual void Start()     {}
    virtual void Update();
    void		setFriction(Friction friction) { m_friction = friction; }
    void		setMass(float mass)            { m_mass = mass; }
    void        SetEnter(bool isEnter)         { m_isEnter = isEnter; }
    void        Bounce(Rigidbody& t1, Rigidbody& t2);
};

#endif
