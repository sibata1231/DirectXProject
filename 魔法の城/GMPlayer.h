#ifndef ___GM_PLAYER_H___
#define ___GM_PLAYER_H___

#include "AnimationModel.h"

namespace GM31 {
    class Player : public Component {
    private:
        int     m_animStack;
        float   m_animFrame;
        std::vector<AnimationModel *> m_modelList;
    public:
        Player();
        ~Player();
        void Start() override;
        void Update()override;
        void Draw()  override;
    };
}

#endif