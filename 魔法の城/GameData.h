#ifndef ___GAME_DATA_H___
#define ___GAME_DATA_H___

#include "Singleton.h"
#include "GraphicsUtility.h"
#include <vector>

// ステージ情報
class StageInfo {
public:
    int             m_id;
    DirectX::XMINT2 m_size;
    bool            m_isClear;
    Vector3         m_startPoint;

};

// ゲームデータ
class GameData : public Singleton<GameData> {
private:
    int m_maxStage;
public:
    std::vector<StageInfo> m_stages;
    void Reset();
};

#endif