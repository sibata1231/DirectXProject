#include "sceneNetWork.h"


#ifdef _CLIENT
#include "Client.h"    
#else
#include "Server.h"
#endif // _CLIENT

SceneNetWork::SceneNetWork(SceneController* sceneManager) :Scene::Scene(sceneManager) {}

void SceneNetWork::Init() {
    Scene::Init();
#ifdef _CLIENT
    Client::GetInstance().ReadSetting();
    Client::GetInstance().Init();
#else
    Server::GetInstance().Init();
#endif // _CLIENT

}

void SceneNetWork::Debug() {
    Scene::Debug();
#ifdef _CLIENT
    Client::GetInstance().DebugLog();
#else
    Server::GetInstance().DebugLog();
#endif // _CLIENT
}

void SceneNetWork::Uninit() {
    Scene::Uninit();
    #ifdef _CLIENT
        Client::GetInstance().Uninit();
    #else
        Server::GetInstance().Uninit();
    #endif // _CLIENT
}

void SceneNetWork::Update() {
	Scene::Update();
#ifdef _CLIENT
    Client::GetInstance().Update();
#else
    Server::GetInstance().Update();
#endif // _CLIENT
}

void SceneNetWork::Draw() {
	Scene::Draw();

#ifdef _CLIENT
    Client::GetInstance().Draw();
#else
    Server::GetInstance().Draw();
#endif // _CLIENT
}
