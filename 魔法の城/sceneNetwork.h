#ifndef _SCENE_NETWORK_H_
#define _SCENE_NETWORK_H_

#include "scene.h"

//#define _CLIENT

class SceneNetWork : public Scene {
public:
            SceneNetWork(SceneController* sceneManager);
    void    Init()   override;
    void    Uninit() override;
    void    Debug()  override;
	void	Update() override;
	void	Draw()   override;
};

#endif