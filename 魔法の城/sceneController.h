#ifndef ___SCENE_CONTROLLER_H___
#define ___SCENE_CONTROLLER_H___
#include "Singleton.h"
enum class LoadModes {
    SINGLE = 0,
    ADDITIVE,
};
class Scene;
class SceneController {
public:
	virtual		 ~SceneController() = 0;
    virtual void ChangeScene(Scene* scene, LoadModes load = LoadModes::SINGLE) = 0;
};

#endif