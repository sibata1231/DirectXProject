#ifndef ___TERRAIN_H___
#define ___TERRAIN_H___

#include "Mesh.h"

class Terrain :public Mesh {
private:
    POMBuffer                 m_pom;
    ID3D11Buffer*             m_pomBuffer;
    ID3D11Buffer*             m_instanceBuffer;
    ID3D11ShaderResourceView* m_bumpTexture;
    float                     m_animationTime;
private:
    HRESULT MakeMeshVertex(POM_VERTEX vertexWk[], int indexWk[]);
    Vector3 CalcTangent(POM_VERTEX p0, POM_VERTEX p1, POM_VERTEX p2);
public:
    virtual ~Terrain() {
        SAFE_RELEASE(m_pomBuffer);
        SAFE_RELEASE(m_instanceBuffer);
    }
    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr);
    virtual HRESULT CreateBumpMapTexture(const char* szPassName);
    virtual void    Start();
    virtual void    Update();
    virtual void    Debug();
    virtual void    Draw();
};

#endif