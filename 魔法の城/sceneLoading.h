#ifndef ___SCENE_LOADING_H___
#define ___SCENE_LOADING_H___

#include "scene.h"

class SceneLoading : public Scene {
private:
    Transform* m_playerTransform;
public:
			SceneLoading();
	void	Init()   override;
	void	Uninit() override;
	void	Update() override;
	void	Draw()   override;
    void    Debug()  override;
};

#endif