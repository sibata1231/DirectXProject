#ifndef ___SCENE_H___
#define ___SCENE_H___

#include "sceneController.h"
#include "Object.h"
#include "imgui.h"

class Scene{
public:
    std::string      m_sceneName;
    bool             m_isActive;
protected:
    SceneController* m_sceneManager;
    ObjectManager    m_objectManager;
public:
					Scene(SceneController* sceneManager);
	virtual			~Scene();
	virtual void	Init()   ;
	virtual	void	Uninit() ;
	virtual void	Update() ;
	virtual void	Draw()   ;
    virtual void    Debug()  ;
};

#endif