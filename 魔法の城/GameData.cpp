#include "GameData.h"
#include "GameData.h"
#include "Panel.h"
#include "Player.h"

void GameData::Reset() {
    PanelManager::GetInstance().Reset();
    PlayerController::GetInstance().Reset(m_stages[0].m_startPoint);
    m_stages[0].m_isClear = false;
}