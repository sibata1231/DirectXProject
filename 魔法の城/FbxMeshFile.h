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
	* @brief モデル読み込み@n
	* モデルデータを読み込む
	* @return 作成の成否 成功(true)
	* @param[in] device デバイス
	* @param[in] vertex_shader モデルと関連付けるVertexShader
	*/
	bool Load(const char* file_name, ID3D11Device *device, VertexShader* vertex_shader);

    /**
     * @brief モデル描画関数@n
     * 指定された情報でモデルを描画する
     * @return 作成の成否 成功(true)
     * @param[in] graphics 描画管理クラス
     * @param[in] pos 描画座標
     * @param[in] scale 拡縮率
     * @param[in] degree 角度
     */
    void Render(DirectGraphics* graphics, Vector3 pos, Vector3 scale, Vector3 degree);

private:
	struct MeshData
	{
		ID3D11Buffer* m_VertexBuffer;			//!< @brief 頂点バッファ(Shader送信用)
		ID3D11Buffer* m_IndexBuffer;			//!< @brief インデックスバッファ(Shader送信用)
		std::vector<CustomVertex> m_Vertices;
		std::vector<UINT> m_Indices;
		std::string m_MaterialName;
		std::string m_UVSetName;
	};

private:
	/**
	* @brief メッシュ生成@n
	* ファイルからメッシュを生成する
	* @return 作成の成否 成功(true)
	* @param[in] file_name ファイル名
	*/
	bool LoadFbxFile(std::vector<std::string>& out_material_list, const char* file_path, const char* file_name);

	/**
	* @brief VertexBuffer作成@n
	* VertexBufferを作成する
	* @return 作成の成否 成功(true)
	* @param[in] device DirectX11グラフィクスデバイス
	*/
	bool CreateVertexBuffer(ID3D11Device* device);

	/**
	* @brief IndexBuffer作成@n
	* IndexBufferを作成する
	* @return 作成の成否 成功(true)
	* @param[in] device DirectX11グラフィクスデバイス
	*/
	bool CreateIndexBuffer(ID3D11Device* device);

	/**
	* @brief メッシュデータを作成する
	* @param[in] node_name ノードの名前
	* @param[in] mesh Meshを作るためのNodeのMeshデータ
	*/
	void CreateMesh(FbxMesh* mesh);


	/**
	* @brief 頂点インデックスデータを読み込む
	* @param[out] mesh_data バッファ保存用
	* @param[in] mesh バッファ取得をするMeshデータ
	*/
	void LoadIndices(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief 頂点データを読み込む
	* @param[out] mesh_data バッファ保存用
	* @param[in] mesh バッファ取得をするMeshデータ
	*/
	void LoadVertices(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief 法線データを読み込む
	* @param[out] mesh_data バッファ保存用
	* @param[in] mesh バッファ取得をするMeshデータ
	*/
	void LoadNormals(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief 頂点カラーデータを読み込む
	* @param[out] mesh_data バッファ保存用
	* @param[in] mesh バッファ取得をするMeshデータ
	*/
	void LoadColors(MeshData& mesh_data, FbxMesh* mesh);

	void LoadUV(MeshData& mesh_data, FbxMesh* mesh);

	/**
	* @brief マテリアルデータを読み込む
	* @param[out] mesh_data バッファ保存用
	* @param[in] mesh バッファ取得をするMeshデータ
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
