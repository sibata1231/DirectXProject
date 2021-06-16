#ifndef ___SCENE_TITLE_H___
#define ___SCENE_TITLE_H___

#include "scene.h"
#include "Object.h"
#include <vector>
#include <memory>
#include "Terrain.h"
#include "LeftWall.h"
#include "RightWall.h"
#include "FxSnow.h"
#include "FxSmoke.h"

#include "Sky.h"
#include "SkyLava.h"

enum class Stages {
    RIVER = 0,
    MAGUMA,
    SNOW,
};

class SceneTitle : public Scene {
private:
    Stages                m_stage;
    Stages                m_preStages;
    int                   m_stageCount;
    std::vector<Terrain*> m_terrainList;
    //LeftWall              *m_riverLeftWall;
    //RightWall             *m_riverRightWall;
    FxSnow                *m_snow;
    FxSmoke               *m_smoke;
    Sky                   *m_sky;
public:
			SceneTitle();
	void	Init()   override;
	void	Uninit() override;
	void	Update() override;
	void	Draw()   override;
    void    Debug()  override;
};

#endif