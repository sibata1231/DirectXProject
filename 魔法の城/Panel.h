#ifndef ___PANEL_H___
#define ___PANEL_H___

#include <list>
#include "Mesh.h"
#include "Singleton.h"

class Panel :public Mesh {
private:
    POMBuffer                 m_pom;
    ID3D11Buffer*             m_pomBuffer;
    ID3D11ShaderResourceView* m_bumpTexture;
    float                     m_animationTime;
    Vector3                   m_startPosition;
private:
    HRESULT MakeMeshVertex(POM_VERTEX vertexWk[], int indexWk[]);
    DirectX::XMFLOAT3 CalcTangent(POM_VERTEX p0, POM_VERTEX p1, POM_VERTEX p2);
public:
    int m_id;
public:
    virtual ~Panel() {
        SAFE_RELEASE(m_pomBuffer);
    }
    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr);
    virtual HRESULT CreateBumpMapTexture(const char* szPassName);
    virtual void    Start();
    virtual void    Update();
    virtual void    Debug();
    virtual void    Draw();

    void Reset();
};

class PanelManager : public Singleton<PanelManager> {
private:
    std::list<Panel*> m_map;
public:
    int               m_count;
private:
    Panel* CheckUp(Vector3 pos);
    void   Add(Panel* panel);
public:
    void   Init(ObjectManager* objectManager);
    void   CreateStage(int stageNo);
    XMINT2 StageSize();
    void   Reset();
    bool   IsCheck(Vector3 pos);
    void   Delete(Vector3 pos);
};

#endif