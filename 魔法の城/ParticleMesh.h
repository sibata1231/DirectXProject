#ifndef ___PARTICLE_MESH_H___
#define ___PARTICLE_MESH_H___

#include "Mesh.h"

enum class AnimationMode {
    NONE,
    ONCE_PLAY,
    ONCE_REPLAY,
    LOOP_PLAY,
    LOOP_REPLAY,
    Max,
};

struct ParticleAnimation {
    AnimationMode	m_mode;
    float			m_count;
    float			m_frame;
    XMINT2			m_size;
};
// 頂点フォーマット( 頂点座標[3D] / 法線ベクトル / 反射光 / テクスチャ座標 )
typedef struct {
    XMFLOAT3 vtx;		//!< 頂点座標
    XMFLOAT3 nor;		//!< 法線ベクトル
    XMFLOAT4 diffuse;	//!< 拡散反射光
    XMFLOAT2 tex;		//!< テクスチャ座標
} MESH_VERTEX;

class ParticleMesh : public Mesh {
private:
    ID3D11Buffer*   m_instanceBuffer;
private:
    HRESULT MakeMeshVertex(MESH_VERTEX vertexWk[], int indexWk[]);
public:
    ~ParticleMesh() {
        SAFE_RELEASE(m_instanceBuffer);
    }

    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr);
    virtual HRESULT Create(ID3D11ShaderResourceView* texture, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr);
    virtual void    Start();
    virtual void    Update();
    virtual void    Draw();
    void            SetVertex(ParticleAnimation anim);
};

#endif