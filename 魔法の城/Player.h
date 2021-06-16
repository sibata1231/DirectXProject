#ifndef ___PLAYER_H___
#define ___PLAYER_H___

#include "Model.h"

class Player : public Component {
private :
    ModelRenderer *m_renderer;
public:
    Player();
    ~Player();
    void Start()  override;
    void Update() override;
    void Draw()   override;
    void Reset(Vector3 pos) {
        m_transform->m_position = pos;
        m_transform->m_rotation = Vector3(-90.0f, 180.0f, 0.0f);
        m_transform->ResetTween();
    }
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
};

class PlayerController : public Singleton<PlayerController> {
private:
    Player* m_player;
public:
    void Register(Player* player);
    void Input();
    void Reset(Vector3 pos) {
        m_player->Reset(pos);
    }
};

#endif