#ifndef ___CAMERA_H___
#define ___CAMERA_H___

#include "Singleton.h"
#include "Main.h"
#include "GraphicsUtility.h"
#include <map>

class Camera {
protected:
    Vector3    m_nowEye;       // 現在の視点
    Vector3    m_nowLook;      // 現在の注視点
    Vector3    m_nowUp;        // 現在の上方ベクトル
public:
    Vector3    m_eyePosition;  // 視点座標
    Vector3    m_lookPosition; // 注視点座標
    Vector3    m_upVector;     // 上方ベクトル
    XMFLOAT4X4 m_view;         // ビュー変換
    XMFLOAT4X4 m_projection;   // 射影変換
    float      m_fovy;         // 視野角
    float      m_nearZ;        // 前方クリップ距離
    float      m_farZ;         // 後方クリップ距離
    float      m_aspext;       // アスペクト比
public:
    virtual void Init();
    virtual void Uninit();
    virtual void Update();
};

class MainCamera :public Singleton<MainCamera> {
private:
    std::map<int, Camera* > m_cameraIndex;
    Camera*                 m_mainCamera;      // メインカメラ
    int                     m_indexCount;      // インデックスカウント
public :
    bool                    m_isController;
public:
    XMFLOAT4X4& GetView()                 { return m_mainCamera->m_view; }
    XMFLOAT4X4& GetProjection()           { return m_mainCamera->m_projection; }
    Vector3&    GetEye()                  { return m_mainCamera->m_eyePosition; }
    Vector3&    GetLook()                 { return m_mainCamera->m_lookPosition; }
    Vector3&    GetUpVector()             { return m_mainCamera->m_upVector; }
    float&      GetFar()                  { return m_mainCamera->m_farZ; }
    float&      GetNear()                 { return m_mainCamera->m_nearZ; }
    void        SetLook(Vector3 look)     { m_mainCamera->m_lookPosition = look; }
    void        Init();
    void        Uninit();
    void        Update();
    void        Add(Camera *camera);
    void        SetTarget(int index);
};

#endif