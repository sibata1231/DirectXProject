#ifndef ___SCENE_GAME_H___
#define ___SCENE_GAME_H___

#include "scene.h"

class SceneGame : public Scene {
public:
		 SceneGame();
	void Init() override;
	void Uninit() override;
    void Debug() override;
	void Update() override;
	void Draw() override;
};

#endif