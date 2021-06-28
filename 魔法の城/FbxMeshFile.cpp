#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <codecvt> 
#include <cstdio>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include "DirectGraphics.h"
#include "Utility.h"
#include "FbxMeshFile.h"
#include "Texture.h"

bool FbxMeshFile::Load(const char* file_name, ID3D11Device *device, VertexShader* vertex_shader) {
	char file_path[256];
	std::vector<std::string> out_material_list;
	int len = (int)strlen(file_name);
	int path_tail_point = 0;

	for (int i = len - 1; i >= 0; i--) {
		if (file_name[i] == '/') {
			path_tail_point = i;
			break;
		}
	}

	strncpy_s(file_path, file_name, path_tail_point + 1);

	if (LoadFbxFile(out_material_list, file_path, file_name) == false) {
		return false;
	}

	if (CreateVertexBuffer(device) == false){
		return false;
	}

	if (CreateIndexBuffer(device) == false) {
		return false;
	}

	return true;
}

void FbxMeshFile::Render(DirectGraphics* graphics, Vector3 pos, Vector3 scale, Vector3 degree) {
	UINT strides = sizeof(CustomVertex);
	UINT offsets = 0;
    
	for (auto& mesh : m_MeshList) {
		// IAに設定する頂点バッファの指定
		graphics->GetContext()->IASetVertexBuffers(
				0,								// バッファ送信のスロット番号
				1,								// バッファの数
				&mesh.m_VertexBuffer,	        // 頂点バッファ
				&strides,						// バッファに使用している構造体のサイズ
				&offsets);						// 開始オフセット
        // IAに設定するインデックスバッファの指定
		graphics->GetContext()->IASetIndexBuffer(
				mesh.m_IndexBuffer,
				DXGI_FORMAT_R32_UINT,
				0);

		// ワールドマトリクス設定
		DirectX::XMMATRIX world_matrix;
		DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		DirectX::XMMATRIX rotate_x = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(degree.x));
		DirectX::XMMATRIX rotate_y = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(degree.y));
		DirectX::XMMATRIX rotate_z = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(degree.z));
		DirectX::XMMATRIX scale_mat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

        // 各バッファ更新
        graphics->UpdateWorldMatrixBuffer(world_matrix, world_matrix); // マトリックス
        graphics->UpdateMaterial(m_Materials[mesh.m_MaterialName]);    // マテリアル

		// テクスチャ設定
		if (m_MaterialLinks.count(mesh.m_MaterialName) > 0) {
            graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL, DirectGraphics::TextureData(0, 1, &m_MaterialLinks[mesh.m_MaterialName]));
		} else {
            graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL, DirectGraphics::TextureData(0, 1, nullptr));
		}

		// 描画
		graphics->GetContext()->DrawIndexed(
				(UINT)mesh.m_Indices.size(),// 頂点数
				0,						    // オフセット
				0);						    // 開始頂点のインデックス
	}
}

void FbxMeshFile::LoadVertices(MeshData& mesh_data, FbxMesh* mesh) {
	// 頂点バッファの取得
	FbxVector4* vertices = mesh->GetControlPoints();
	// インデックスバッファの取得
	int* indices = mesh->GetPolygonVertices();
	// 頂点座標の数の取得
	int polygon_vertex_count = mesh->GetPolygonVertexCount();
	// GetPolygonVertexCount => 頂点数
	for (int i = 0; i < polygon_vertex_count; i++) {
		CustomVertex vertex;
		// インデックスバッファから頂点番号を取得
		int index = indices[i];

		// 頂点座標リストから座標を取得する
		vertex.Posision.x = (float)-vertices[index][0];
		vertex.Posision.y = (float)vertices[index][1];
		vertex.Posision.z = (float)vertices[index][2];

		// 追加
		mesh_data.m_Vertices.push_back(vertex);
	}
}

void FbxMeshFile::LoadIndices(MeshData& mesh_data, FbxMesh* mesh) {
	// ポリゴン数の取得
	int polygon_count = mesh->GetPolygonCount();

	// ポリゴンの数だけ連番として保存する
	for (int i = 0; i < polygon_count; i++) {
		mesh_data.m_Indices.push_back(i * 3 + 2);
		mesh_data.m_Indices.push_back(i * 3 + 1);
		mesh_data.m_Indices.push_back(i * 3);
	}
}

void FbxMeshFile::LoadNormals(MeshData& mesh_data, FbxMesh* mesh) {
	FbxArray<FbxVector4> normals;
	// 法線リストの取得
	mesh->GetPolygonVertexNormals(normals);

	// 法線設定
	for (int i = 0; i < normals.Size(); i++) {
		mesh_data.m_Vertices[i].Normal.x = (float)-normals[i][0];
		mesh_data.m_Vertices[i].Normal.y = (float)normals[i][1];
		mesh_data.m_Vertices[i].Normal.z = (float)normals[i][2];
	}
}

void FbxMeshFile::LoadColors(MeshData& mesh_data, FbxMesh* mesh) {
	// 頂点カラーデータの数を確認
	int color_count = mesh->GetElementVertexColorCount();
	if (color_count == 0) {
		return;
	}
	
	// 頂点カラーデータの取得
	FbxGeometryElementVertexColor* vertex_colors = mesh->GetElementVertexColor(0);

	if (vertex_colors == nullptr) {
		return;
	}

	FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
	FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();

	if (mapping_mode == FbxLayerElement::eByPolygonVertex) {
		if (reference_mode == FbxLayerElement::eIndexToDirect) {
			// 頂点カラーバッファ取得
			FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
			// 頂点カラーインデックスバッファ取得
			FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
			for (int i = 0; i < indeces.GetCount(); i++ ){
				int id = indeces.GetAt(i);
				FbxColor color = colors.GetAt(id);
				mesh_data.m_Vertices[i].Color.Alpha = (float)color.mAlpha;
				mesh_data.m_Vertices[i].Color.Red = (float)color.mRed;
				mesh_data.m_Vertices[i].Color.Green = (float)color.mGreen;
				mesh_data.m_Vertices[i].Color.Blue = (float)color.mBlue;
			}
		}
	}
}

void FbxMeshFile::LoadUV(MeshData& mesh_data, FbxMesh* mesh) {
	FbxStringList uvset_names;
	// UVSetの名前リストを取得
	mesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;

	// UVSetの名前からUVSetを取得する
	// 今回はマルチテクスチャには対応しないので最初の名前を使う
	mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++) {
		FbxVector2& uv = uv_buffer[i];

		// そのまま使用して問題ない
		mesh_data.m_Vertices[i].TexturePos.X = (float)uv[0];
		mesh_data.m_Vertices[i].TexturePos.Y = (float)(1.0 - uv[1]);
	}
}

void FbxMeshFile::SetMaterialName(MeshData& mesh_data, FbxMesh* mesh) {
	// マテリアルが無ければ終わり
	if (mesh->GetElementMaterialCount() == 0) {
		mesh_data.m_MaterialName = "";
		return;
	}

	// Mesh側のマテリアル情報を取得
	FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
	int index = material->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* surface_material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);
	if (surface_material != nullptr) {
		mesh_data.m_MaterialName = surface_material->GetName();
	} else {
		mesh_data.m_MaterialName = "";
	}
}

bool FbxMeshFile::LoadTexture(FbxFileTexture* texture, std::string& keyword) {
	if (texture == nullptr) {
		return false;
	}

	// ファイル名を取得
	std::string file_path = texture->GetRelativeFileName();

	// ファイル分解
	char buffer[256];
	ZeroMemory(buffer, sizeof(char) * 256);
	memcpy(buffer, file_path.c_str(), sizeof(char) * 256);

	// 記号統一
	Replace('\\', '/', buffer);
	std::vector<std::string> split_list;
	std::string replace_file_name = buffer;
	// 「/」で分解
	Split('/', buffer, split_list);

	std::string root_path = "Resources/Texture/";
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	std::wstring wstr_file_name = cv.from_bytes(root_path + split_list[split_list.size() - 1]);

	// 文字化け対策
	char* file_name;
	size_t size = 0;
	FbxUTF8ToAnsi(split_list[split_list.size() - 1].c_str(), file_name, &size);

	if (m_Textures.count(file_name) > 0 &&
		m_Textures[file_name] != nullptr) {
		FbxFree(file_name);
		return true;
	}

	if (FAILED(CreateTextureFromFile(
		DirectGraphics::GetInstance().GetDevice(),			// ID3D11Device
		wstr_file_name.c_str(),								// ファイル名(wchar_t型なので注意)
		&m_Textures[file_name])))							// シェーダで使用することができるテクスチャデータ
	{
		FbxFree(file_name);
		return false;
	}

	keyword = file_name;
	FbxFree(file_name);
	return true;
}

void FbxMeshFile::LoadMaterial(FbxSurfaceMaterial* material) {
	ObjMaterial entry_material;
	enum class MaterialOrder {
		Ambient,
		Diffuse,
		Specular,
		MaxOrder,
	};

	FbxDouble3 colors[(int)MaterialOrder::MaxOrder];
	FbxDouble factors[(int)MaterialOrder::MaxOrder];
	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
		const char* element_check_list[] = {
			FbxSurfaceMaterial::sAmbient,
			FbxSurfaceMaterial::sDiffuse,
		};

		const char* factor_check_list[] = {
			FbxSurfaceMaterial::sAmbientFactor,
			FbxSurfaceMaterial::sDiffuseFactor,
		};

		for (int i = 0; i < 2; i++) {
			prop = material->FindProperty(element_check_list[i]);
			if (prop.IsValid()) {
				colors[i] = prop.Get<FbxDouble3>();
			} else {
				colors[i] = FbxDouble3(1.0, 1.0, 1.0);
			}

			prop = material->FindProperty(factor_check_list[i]);
			if (prop.IsValid()) {
				factors[i] = prop.Get<FbxDouble>();
			} else {
				factors[i] = 1.0;
			}
		}
	}

	FbxDouble3 color = colors[(int)MaterialOrder::Ambient];
	FbxDouble factor = factors[(int)MaterialOrder::Ambient];
	entry_material.SetAmbient((float)color[0], (float)color[1], (float)color[2], (float)factor);

	color = colors[(int)MaterialOrder::Diffuse];
	factor = factors[(int)MaterialOrder::Diffuse];
	entry_material.SetDiffuse((float)color[0], (float)color[1], (float)color[2], (float)factor);

	m_Materials[material->GetName()] = entry_material;

	// テクスチャ読み込み(シングル対応)
	// Diffuseプロパティを取得
	prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxFileTexture* texture = nullptr;
	std::string keyword;
	int texture_num = prop.GetSrcObjectCount<FbxFileTexture>();
	if (texture_num > 0) {
		// propからFbxFileTextureを取得	
		texture = prop.GetSrcObject<FbxFileTexture>(0);
	} else {
		// FbxLayeredTextureからFbxFileTextureを取得	
		int layer_num = prop.GetSrcObjectCount<FbxLayeredTexture>();
		if (layer_num > 0) {
			texture = prop.GetSrcObject<FbxFileTexture>(0);
		}
	}

	if (texture != nullptr &&
		LoadTexture(texture, keyword) == true) {
		// 読み込んだテクスチャとマテリアルの関係を覚えておく
		m_MaterialLinks[material->GetName()] = m_Textures[keyword];
	}
}

void FbxMeshFile::CreateMesh(FbxMesh* mesh) {
	MeshData mesh_data;
	LoadIndices(mesh_data, mesh);
	LoadVertices(mesh_data, mesh);
	LoadNormals(mesh_data, mesh);
	LoadUV(mesh_data, mesh);
	LoadColors(mesh_data, mesh);
	SetMaterialName(mesh_data, mesh);

	m_MeshList.push_back(mesh_data);
}

bool FbxMeshFile::LoadFbxFile(std::vector<std::string>& out_material_list, const char* file_path, const char* file_name) {
	// FbxManager作成
	FbxManager* fbx_manager = fbxsdk::FbxManager::Create();
	if (fbx_manager == nullptr) {
		return false;
	}
	
	// FbxImporter作成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	if (fbx_importer == nullptr) {
		fbx_manager->Destroy();
		return false;
	}
	
	// FbxScene作成
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "");
	if (fbx_scene == nullptr) {
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return false;
	}

	// Fileを初期化
	fbx_importer->Initialize(file_name);
	// sceneにインポート
	fbx_importer->Import(fbx_scene);

	FbxGeometryConverter converter(fbx_manager);
	// メッシュに使われているマテリアル単位でメッシュを分割する
	converter.SplitMeshesPerMaterial(fbx_scene, true);
	// ポリゴンを三角形にする
	converter.Triangulate(fbx_scene, true);

	int material_num = fbx_scene->GetSrcObjectCount<FbxSurfaceMaterial>();

	for (int i = 0; i < material_num; i++) {
		LoadMaterial(fbx_scene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	// FbxMeshの数を取得
	int mesh_num = fbx_scene->GetSrcObjectCount<FbxMesh>();

	for (int i = 0; i < mesh_num; i++) {
		// Mesh作成
		CreateMesh(fbx_scene->GetSrcObject<FbxMesh>(i));
	}

	int texture_num = fbx_scene->GetSrcObjectCount<FbxFileTexture>();
	for (int i = 0; i < texture_num; i++) {
		FbxFileTexture* texture = fbx_scene->GetSrcObject<FbxFileTexture>(i);
		if (texture) {
			const char* file_name01 = texture->GetFileName();
			const char* file_name02 = texture->GetRelativeFileName();
			int tex = texture->GetSrcObjectCount< FbxSurfaceMaterial>();
		}
	}

	fbx_importer->Destroy();
	fbx_scene->Destroy();
	fbx_manager->Destroy();

	return true;
}

bool FbxMeshFile::CreateVertexBuffer(ID3D11Device* device) {
	for (auto& mesh : m_MeshList) {
		//頂点バッファ作成
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = (UINT)(sizeof(CustomVertex) * mesh.m_Vertices.size());	// バッファのサイズ
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;			// 使用方法
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// BIND設定
		buffer_desc.CPUAccessFlags = 0;						// リソースへのCPUのアクセス権限についての設定
		buffer_desc.MiscFlags = 0;							// リソースオプションのフラグ
		buffer_desc.StructureByteStride = 0;				// 構造体のサイズ

		D3D11_SUBRESOURCE_DATA sub_resource;
		sub_resource.pSysMem = &mesh.m_Vertices[0];	// バッファの中身の設定
		sub_resource.SysMemPitch = 0;			// textureデータを使用する際に使用するメンバ
		sub_resource.SysMemSlicePitch = 0;		// textureデータを使用する際に使用するメンバ

		// バッファ作成
		if (FAILED(device->CreateBuffer(
			&buffer_desc,						// バッファ情報
			&sub_resource,						// リソース情報
			&mesh.m_VertexBuffer)))	            // 作成されたバッファの格納先
		{
			return false;
		}
	}

	return true;
}

bool FbxMeshFile::CreateIndexBuffer(ID3D11Device* device) {
	for (auto& mesh : m_MeshList) {
		//頂点バッファ作成
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = (UINT)(sizeof(UINT) * mesh.m_Indices.size());	    // バッファのサイズ
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;							// 使用方法
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;					// BIND設定
		buffer_desc.CPUAccessFlags = 0;										// リソースへのCPUのアクセス権限についての設定
		buffer_desc.MiscFlags = 0;											// リソースオプションのフラグ
		buffer_desc.StructureByteStride = 0;								// 構造体のサイズ

		D3D11_SUBRESOURCE_DATA sub_resource;
		sub_resource.pSysMem = &mesh.m_Indices[0];							// バッファの中身の設定
		sub_resource.SysMemPitch = 0;										// textureデータを使用する際に使用するメンバ
		sub_resource.SysMemSlicePitch = 0;									// textureデータを使用する際に使用するメンバ

		// バッファ作成
		if (FAILED(device->CreateBuffer(
			&buffer_desc,						// バッファ情報
			&sub_resource,						// リソース情報
			&mesh.m_IndexBuffer)))		// 作成されたバッファの格納先
		{
			return false;
		}
	}

	return true;
}