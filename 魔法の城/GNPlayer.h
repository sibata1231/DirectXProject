#ifndef ___GN_PLAYER_H___
#define ___GN_PLAYER_H___
#include "Model.h"

class GNPlayer : public Component {
public:
    GNPlayer();
    ~GNPlayer();
    void Start();
    void Update();
    void Draw();
};

#endif // !___GN_PLAYER_H___