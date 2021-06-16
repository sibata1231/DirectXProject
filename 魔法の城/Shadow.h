#ifndef ___SHADOW_H___
#define ___SHADOW_H___

#include "Mesh.h"

class Shadow :public Mesh {
private:
    HRESULT MakeMeshVertex(SHADOW_VERTEX vertexWk[], int indexWk[]);
public:
    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr);
    virtual void    Start();
    virtual void    Update();
    virtual void    Debug();
    virtual void    Draw();
};

#endif