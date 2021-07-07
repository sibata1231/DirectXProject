#ifndef ___TESSELLATION_H___
#define ___TESSELLATION_H___

#include "Mesh.h"

class Tessellation :public Mesh {
private:
    TessellationBuffer        m_tessellation;
    ID3D11Buffer*             m_tessellationBuffer;
    float                     m_animationTime;
private:
    HRESULT MakeMeshVertex(TESSELLATION_VERTEX vertexWk[], UINT indexWk[]);
    void CreateBuffers(TESSELLATION_VERTEX * vertices, int numVertices, unsigned int * indices, int numIndices, ID3D11Device * device);
    void CalculateTangents(TESSELLATION_VERTEX* vertex, int numVerts, UINT* indices, int numIndices);
public:
    virtual ~Tessellation() {
        SAFE_RELEASE(m_tessellationBuffer);
    }
    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr);
    virtual void    Create(const char * objFile, ID3D11Device * device);

    virtual void    Start();
    virtual void    Update();
    virtual void    Debug();
    virtual void    Draw();
};

#endif