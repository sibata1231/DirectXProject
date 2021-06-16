#ifndef ___MESH_H___
#define ___MESH_H___

#include "Transform.h"
#include "MeshRenderer.h"


// Terrain , Panel
typedef struct {
    XMFLOAT3 vtx;		//!< ���_���W
    XMFLOAT3 nor;		//!< �@���x�N�g��
    XMFLOAT2 tex;		//!< �e�N�X�`�����W
    XMFLOAT3 tan;		//!< �ڐ��x�N�g��
    UINT     id;        //!< �C���X�^���XID
} POM_VERTEX;

// Shadow
typedef struct {
    XMFLOAT3 vtx;	//!< ���_���W
    XMFLOAT3 nor;	//!< �@���x�N�g��
    XMFLOAT4 color; //!< �J���[
    XMFLOAT2 tex;	//!< �e�N�X�`�����W
} SHADOW_VERTEX;

class Mesh : public Component {
protected:
    HRESULT MakeTexture(const char*szPassName);
public:
	ID3D11Buffer*                 m_vertexBuffer;	// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	ID3D11Buffer*                 m_indexBuffer;	// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	int                           m_vertexNum;		// �����_��	
	int                           m_indexNum;		// ���C���f�b�N�X��
    XMFLOAT4X4                    m_mtxTexture;		// �e�N�X�`�� �}�g���b�N�X
    PrimitiveTypes                m_primitiveType;	// �v���~�e�B�u�^
    ObjMaterial*                  m_material;		// �}�e���A��
    ID3D11ShaderResourceView*     m_texture;		// �e�N�X�`��
    MeshRenderer*                 m_renderer;       // �����_���[
public:
    virtual void Release();

	Mesh();
    virtual ~Mesh();
    virtual HRESULT Create(const char* szPassName, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr) { return S_OK; }
    virtual HRESULT Create(ID3D11ShaderResourceView* texture, PrimitiveTypes primitiveType = PrimitiveTypes::TRIANGLESTRIP, ObjMaterial* matrial = nullptr) { return S_OK; }
    virtual void    Start();
    virtual void    Update();
    virtual void    Draw();
    int inline GetVertex() { return m_vertexNum; }
	int inline GetIndex()  { return m_indexNum;  }
    ID3D11ShaderResourceView* GetTexture() { return m_texture; }
    void SetTexture(ID3D11ShaderResourceView* texture) { m_texture = texture; }
};

#endif