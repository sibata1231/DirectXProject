#ifndef ___SCENE_TITLE_H___
#define ___SCENE_TITLE_H___

#include "scene.h"

class SceneTitle : public Scene {
public:
			SceneTitle();
	void	Init()   override;
	void	Uninit() override;
	void	Update() override;
	void	Draw()   override;
    void    Debug()  override;
};

#endif