#ifndef ___PLAYER_H___
#define ___PLAYER_H___

#include "Model.h"

class Player : public Component {
public:
    Player();
    ~Player();
    void Start()  override;
    void Update() override;
    void Draw()   override;
};

#endif