#ifndef ___ENEMY_H___
#define ___ENEMY_H___
#include "Model.h"
#include "Corutine.h"

enum class EnemyStates {
    NONE = 0,
    MOVE,
    DEATH,
};

class Enemy : public Component {
private:
    std::future<int> m_move;   
    EnemyStates      m_states;   
    float            m_speed;
    int              m_nextNo;
    Vector3         m_nextPos;
public:
    int m_fieldNo;
private:
    std::future<int> MoveAsync();
public:
    Enemy();
    ~Enemy();
    void Start() override;
    void Update()override;
    void Draw()  override;
    void OnCollisionEnter(Object * object) override;
    void RootCheck();
    //bool calcRaySphere(
    //    float lx, float ly, float lz,
    //    float vx, float vy, float vz,
    //    float px, float py, float pz,
    //    float r,
    //    float &q1x, float &q1y, float &q1z,
    //    float &q2x, float &q2y, float &q2z);
};

#endif