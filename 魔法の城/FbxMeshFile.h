#ifndef FBX_MESH_FILE_H_
#define FBX_MESH_FILE_H_

#include <fbxsdk.h>
#include <vector>
#include <map>
#include <d3d11.h>
#include "DirectGraphics.h"
#include "VertexShader.h"
#include "GraphicsUtility.h"

#pragma comment(lib, "libfbxsdk-mt")
#pragma comment(lib, "libxml2-mt")
#pragma comment(lib, "zlib-mt")

class FbxMeshFile {
public:
	/** Constructor */
	FbxMeshFile() {
		m_MeshList.clear();
	}

	/** Destructor */
	~FbxMeshFile()
	{
		for (auto texture : m_Textures)
		{
			if (texture.second != nullptr)
			{
				texture.second->Release();
			}
		}
		m_Textures.clear();
		m_MaterialLinks.clear();

		for (auto mesh : m_MeshList)
		{
			if (mesh.m_VertexBuffer != nullptr)
			{
				mesh.m_VertexBuffer->Release();
			}

			if (mesh.m_IndexBuffer != nullptr)
			{
				mesh.m_IndexBuffer->Release();
			}
			mesh.m_Indices.clear();
			mesh.m_Vertices.clear();
		}
	}

	/**
	* @brief ���f���ǂݍ���@n
	* ���f���f�[�^��ǂݍ���
	* @return �쐬�̐��� ����(true)
	* @param[in] device �f�o�C�X
	* @param[in] vertex_shader ���f���Ɗ֘A�t����VertexShader
	*/
	bool Load(const char* file_name, ID3D11Device *device, VertexShader* vertex_shader);

    /**
     * @brief ���f���`��֐�@n
     * �w�肳�ꂽ���Ń��f����`�悷��
     * @return �쐬�̐��� ����(true)
     * @param[in] graphics �`��Ǘ��N���X
     * @param[in] pos �`����W
     * @param[in] scale �g�k��
     * @param[in] degree �p�x
     */
    void Render(DirectGraphics* graphics, Vector3 pos, Vector3 scale, Vector3 degree);

private:
	struct MeshData
	{
		ID3D11Buffer* m_VertexBuffer;			//!< @brief ���_�o�b�t�@(Shader���M�p)
		ID3D11Buffer* m_IndexBuffer;			//!< @brief �C���f�b�N�X�o�b�t�@(Shader���M�p)
		std::vector<CustomVertex> m_Vertices;
		std::vector<UINT> m_Indices;
		std::string m_MaterialName;
		std::string m_UVSetName;
	};

private:
	/**
	* @brief ���b�V������@n
	* �t�@�C�����烁�b�V���𐶐�����
	* @return �쐬�̐��� ����(true)
	* @param[in] file_name �t�@�C����
	*/
	bool LoadFbxFile(std::vector<std::string>& out_material_list, const char* file_path, const char* file_name);

	/**
	* @brief VertexBuffer�쐬@n
	* VertexBuffer���쐬����
	* @return �쐬�̐��� ����(true)
	* @param[in] device DirectX11�O���t�B�N�X�f�o�C�X
	*/
	bool CreateVertexBuffer(ID3D11Device* device);

	/**
	* @brief IndexBuffer�쐬@n
	* IndexBuffer���쐬����
	* @return �쐬�̐��� ����(true)
	* @param[in] device DirectX11�O���t�B�N�X�f�o�C�X
	*/
	bool CreateIndexBuffer(ID3D11Device* device);

	/**
	* @brief ���b�V���f�[�^���쐬����
	* @param[in] node_name �m�[�h�̖��O
	* @param[in] mesh Mesh����邽�߂�Node��Mesh�f�[�^
	*/
	void CreateMesh(FbxMesh* mesh);


	/**
	* @brief ���_�C���f�b�N�X�f�[�^��ǂݍ���
	* @param[out] mesh_data �o�b�t�@�ۑ��p
	* @param[in] mesh �o�b�t�@�擾������Mesh�f�[�^
	*/
	void LoadIndices(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief ���_�f�[�^��ǂݍ���
	* @param[out] mesh_data �o�b�t�@�ۑ��p
	* @param[in] mesh �o�b�t�@�擾������Mesh�f�[�^
	*/
	void LoadVertices(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief �@���f�[�^��ǂݍ���
	* @param[out] mesh_data �o�b�t�@�ۑ��p
	* @param[in] mesh �o�b�t�@�擾������Mesh�f�[�^
	*/
	void LoadNormals(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief ���_�J���[�f�[�^��ǂݍ���
	* @param[out] mesh_data �o�b�t�@�ۑ��p
	* @param[in] mesh �o�b�t�@�擾������Mesh�f�[�^
	*/
	void LoadColors(MeshData& mesh_data, FbxMesh* mesh);

	void LoadUV(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief �}�e���A���f�[�^��ǂݍ���
	* @param[out] mesh_data �o�b�t�@�ۑ��p
	* @param[in] mesh �o�b�t�@�擾������Mesh�f�[�^
	*/
	void LoadMaterial(FbxSurfaceMaterial* material);

	bool LoadTexture(FbxFileTexture* material, std::string& keyword);

	void SetMaterialName(MeshData& mesh_data, FbxMesh* mesh);


private:
	std::vector<MeshData>                            m_MeshList;
	std::map<std::string, ObjMaterial>               m_Materials;
	std::map<std::string, ID3D11ShaderResourceView*> m_Textures;
	std::map<std::string, ID3D11ShaderResourceView*> m_MaterialLinks;
};

#endif
