#include "Enemy.h"
#include "Camera.h"
#include "Light.h"
#include "input.h"
#include "Debugproc.h"
#include "FieldManager.h"

#define PATH_ENEMY ("data/model/Dragon.fbx") // モデル
#define SPEED       (15.0f)                  // 速さ

Enemy:: Enemy() {}
Enemy::~Enemy() {
    m_states = EnemyStates::DEATH;
    m_move.wait();
}

// 初期化
void Enemy::Start() {
    Model* model = m_parent->AddComponentIf<Model>();
    model->Create(ModelIndex::ENEMY, PATH_ENEMY);
    m_transform->m_name    = "Enemy";
    m_transform->m_tagName = "Enemy";
    m_transform->m_rotation.y = XMConvertToRadians(180.0f);
    m_speed = 0.1f;
    m_states = EnemyStates::NONE;
}

// 更新
void Enemy::Update() {}

// 描画
void Enemy::Draw() {}

void Enemy::OnCollisionEnter(Object* object) {
    if (object->m_transform->GetName() == "SampleObject") {
        XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);
    }
}

void Enemy::RootCheck() {
//    m_nextNo = THE_FIELD_MANAGER.NextPosition(m_fieldNo);
    m_nextPos = THE_FIELD_MANAGER.GetFieldPosition(m_nextNo);
    if (m_nextNo == -1) {
#ifdef _DEBUG
        printf("フィールドから削除 %d \n ", m_fieldNo);
#endif
        THE_FIELD_MANAGER.RemoveData(m_fieldNo, m_parent);
    }
#ifdef _DEBUG
    printf("EnemyCurrentNo&Pos([%d] %.3f,%.3f,%.3f),EnemyNextNoPos([%d] %.3f,%.3f,%.3f)\n",
        m_fieldNo, m_transform->m_position.x, m_transform->m_position.y, m_transform->m_position.z,
        m_nextNo, m_nextPos.x, m_nextPos.y, m_nextPos.z);
#endif
    m_states = EnemyStates::MOVE;
    m_move = MoveAsync();
}

std::future<int> Enemy::MoveAsync() {
    int i = 0;
    while (m_states != EnemyStates::DEATH) {
        i++;
        Vector3 diffPos = m_transform->m_position;
        Vector3 pos = m_transform->m_position;
        diffPos.x -= m_nextPos.x;
        diffPos.z -= m_nextPos.z;
        if (diffPos.x == 0.0f && diffPos.z <= 1.0f) {
            m_fieldNo = THE_FIELD_MANAGER.MoveData(m_fieldNo, m_parent);
//            m_nextNo  = THE_FIELD_MANAGER.NextPosition(m_fieldNo);
            m_nextPos = THE_FIELD_MANAGER.GetFieldPosition(m_nextNo);
            if (m_nextNo == -1) {
#ifdef _DEBUG
                printf("フィールドから削除 %d \n ", m_fieldNo);
#endif
                THE_FIELD_MANAGER.RemoveData(m_fieldNo, m_parent);
            }
            diffPos = m_transform->m_position;
            diffPos.x -= m_nextPos.x;
            diffPos.z -= m_nextPos.z;
        }
        if (m_fieldNo == -1 || static_cast<int>(diffPos.z) != 0) {
            pos.z -= m_speed;
        }
        else if (static_cast<int>(diffPos.x) != 0) {
            pos.x -= m_speed * ((diffPos.x - m_nextPos.x < 0) ? -1 : 1);
        }
        m_transform->SetPos(pos);
        if (pos.z < -250.0f) {
            m_states = EnemyStates::DEATH;
            THE_OBJECT_MANAGER.Delete(m_parent->m_id);
        }
        auto n = co_await std::async(std::launch::async, [i]() -> int {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return i;
        });
    }
    return 0;
}
//bool Enemy::calcRaySphere(
//    float lx, float ly, float lz,
//    float vx, float vy, float vz,
//    float px, float py, float pz,
//    float r,
//    float &q1x, float &q1y, float &q1z,
//    float &q2x, float &q2y, float &q2z) {
//    px = px - lx;
//    py = py - ly;
//    pz = pz - lz;
//
//    float A = vx * vx + vy * vy + vz * vz;
//    float B = vx * px + vy * py + vz * pz;
//    float C = px * px + py * py + pz * pz - r * r;
//
//    if (A == 0.0f)
//        return false; // レイの長さが0
//
//    float s = B * B - A * C;
//    if (s < 0.0f)
//        return false; // 衝突していない
//
//    s = sqrtf(s);
//    float a1 = (B - s) / A;
//    float a2 = (B + s) / A;
//
//    if (a1 < 0.0f || a2 < 0.0f)
//        return false; // レイの反対で衝突
//
//    q1x = lx + a1 * vx;
//    q1y = ly + a1 * vy;
//    q1z = lz + a1 * vz;
//    q2x = lx + a2 * vx;
//    q2y = ly + a2 * vy;
//    q2z = lz + a2 * vz;
//
//    return true;
//}
