#ifndef ___PARTICLE_H___
#define ___PARTICLE_H___

#include "RigidBody.h"
#include "ParticleMesh.h"


class Particle : public Component {
private:
    XMFLOAT3		  m_prePos;		  // 旧座標
    float			  m_lifeTime;	  // 消失時間(何フレーム後に消失するのか)
    ParticleAnimation m_animation;    // アニメーションデータ
    Rigidbody*        m_rigidbody;    // 物理用
public:
                    Particle() {};
    virtual			~Particle() {};
    void            Start()  override;
	void			Update() override;
	void			Draw()   override;

private:
    void			Animation();
public:
    // Common
    void inline 	SetOldPos(XMFLOAT3 pos) { m_prePos = pos; }
    void inline     SetLifeTime(float time) { m_lifeTime = time; }
    void inline     SetAnimationMode(AnimationMode animationMode) { m_animation.m_mode = animationMode; }
    void inline     SetAnimationFrame(float animationTime) { m_animation.m_frame = animationTime; }
    void inline     SetAnimationSize(XMINT2 animationSize) { m_animation.m_size = animationSize; }
    void inline     SetActive() { m_isEnable = true; }
    XMFLOAT3 inline	GetOldPos() { return m_prePos; }
    float inline    GetLifeTime() { return m_lifeTime; }
    bool inline     GetActive() { return m_isEnable; }
    void inline     Delete() { m_isEnable = false; }

    // Transform
	void inline     SetPosition(Vector3 pos)    { m_transform->m_position = pos; }
    void inline     SetSize(Vector3 size)       { m_transform->m_scale    = size; }
    void inline     SetRotation(Vector3 rotate) { m_transform->m_rotation = rotate; }

    // RigidBody
    void inline     SetSpeed(Vector3 speed) { m_rigidbody->m_velocity = speed; }
    void inline     SetGravity(bool isGravity) { m_rigidbody->m_isGravity = isGravity; }
};

#endif