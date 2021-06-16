#ifndef ___SceneManager_H___
#define ___SceneManager_H___

#include "sceneController.h"
#include "scene.h"

#include <map>
#include <list>
#include <memory>
#include <future>

#define DEFALUT_SCENE ("Title")

class SceneManager :public SceneController ,public Singleton<SceneManager>{
private:
	std::list<Scene *>     m_scenes;
    bool                   m_isLoadScene;
    Scene*                 m_loadScene;
    LoadModes              m_loadMode;
    std::unique_ptr<Scene> m_fadeScene;    
    bool                   m_isFadeTween;

public:
		 SceneManager();
    void Active();
    void Inactive();

    const bool IsPlay() {
        return m_isFadeTween;
    }
    std::future<int> DOFade();
	void Init(Scene *scene);
	void Uninit();
	void Update();
    void Debug();
	void Draw();
    void ChangeScene(Scene* scene, LoadModes load = LoadModes::SINGLE) override;
};

#endif