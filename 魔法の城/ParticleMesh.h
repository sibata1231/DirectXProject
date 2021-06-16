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
// ���_�t�H�[�}�b�g( ���_���W[3D] / �@���x�N�g�� / ���ˌ� / �e�N�X�`�����W )
typedef struct {
    XMFLOAT3 vtx;		//!< ���_���W
    XMFLOAT3 nor;		//!< �@���x�N�g��
    XMFLOAT4 diffuse;	//!< �g�U���ˌ�
    XMFLOAT2 tex;		//!< �e�N�X�`�����W
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