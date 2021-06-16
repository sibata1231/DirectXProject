#ifndef ___SCENE_ANIMATION_H___
#define ___SCENE_ANIMATION_H___

#include "scene.h"
#include "Object.h"

class SceneAnimation : public Scene {
public:
			SceneAnimation(SceneController* sceneManager);
	void	Init()   override;
	void	Uninit() override;
	void	Update() override;
	void	Draw()   override;
    void    Debug()  override;
};

#endif