#include "Model.h"
#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <cassert>
#include <iostream>

void Model::ParseNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ParseMesh(mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ParseNode(node->mChildren[i], scene);
	}
}

void Model::ParseMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces * 3);

	bool has_normal = (mesh->mNormals != nullptr);
	bool has_texcoord = (mesh->mTextureCoords[0] != nullptr);
	bool has_tangent = (mesh->mTangents != nullptr);
	bool has_bitangent = (mesh->mBitangents != nullptr);
	
	// vertices && normal && texcoord
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		Vertex v;
		v.position.x = mesh->mVertices[i].x;
		v.position.y = mesh->mVertices[i].y;
		v.position.z = mesh->mVertices[i].z;

		if (has_normal) {
			v.normal.x = mesh->mNormals[i].x;
			v.normal.y = mesh->mNormals[i].y;
			v.normal.z = mesh->mNormals[i].z;
		}

		if (has_texcoord) {
			v.texcoord.x = mesh->mTextureCoords[0][i].x;
			v.texcoord.y = mesh->mTextureCoords[0][i].y;
		}

		if (has_tangent) {
			v.tangent.x = mesh->mTangents[i].x;
			v.tangent.y = mesh->mTangents[i].y;
			v.tangent.z = mesh->mTangents[i].z;
		}

		if (has_bitangent) {
			v.bitangent.x = mesh->mBitangents[i].x;
			v.bitangent.y = mesh->mBitangents[i].y;
			v.bitangent.z = mesh->mBitangents[i].z;
		}

		vertices.emplace_back(v);
	}

	// indices
	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const auto& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);

		indices.emplace_back(face.mIndices[0]);
		indices.emplace_back(face.mIndices[1]);
		indices.emplace_back(face.mIndices[2]);
	}

	if (!has_normal)
	{
		ComputeNormal(vertices, indices);
	}

	if ((!has_tangent || !has_bitangent) && has_texcoord)
	{
		ComputeTangent(vertices, indices);
	}

	meshes.emplace_back(vertices, indices, mesh->mMaterialIndex);

	//// 粗略查看一下material都有啥（其实也看不到啥。。。）
	//auto material = scene->mMaterials[mesh->mMaterialIndex];
	//for (int i = 0; i < material->mNumProperties; ++i) {
	//	auto prop = material->mProperties[i];
	//	int a = 333;
	//}
}

void Model::ParseMaterial(const aiScene* scene)
{
	if (scene->HasMaterials() == false) return;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		auto& material = scene->mMaterials[i];
		std::shared_ptr<Material> our_mat = std::make_shared<Material>();

		LoadParams(our_mat, ColorType::AMBIENT, material);
		LoadParams(our_mat, ColorType::DIFFUSE, material);
		LoadParams(our_mat, ColorType::SPECULAR, material);
		
		// 加载这个material对应的不同种类的texture 见 aiTextureType
		LoadTypeTexture(our_mat, aiTextureType_DIFFUSE, material);
		LoadTypeTexture(our_mat, aiTextureType_SPECULAR, material);
		LoadTypeTexture(our_mat, aiTextureType_NORMALS, material);
		LoadTypeTexture(our_mat, aiTextureType_HEIGHT, material); // normal map in .obj file
		// TODO
		materials[i] = our_mat;
	}

}

void Model::LoadTypeTexture(std::shared_ptr<Material> our_mat, aiTextureType type, aiMaterial* material)
{
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;  // path to texture
		material->GetTexture(type, i, &str);
		std::string path(str.C_Str());
		path = dir + '/' + path;
		if (textures.find(path) != textures.end()) {
			// 不重复加载纹理
			our_mat->Set(type, std::make_shared<Texture2D>(textures[path]));
			continue;
		}
	
		Texture2D tex(path.c_str());
		textures[path] = tex;

		our_mat->Set(type, std::make_shared<Texture2D>(tex));  // 给material和texture建立联系
	}
}

void Model::LoadParams(std::shared_ptr<Material> our_mat, ColorType type, aiMaterial* material)
{
	aiColor3D ret;
	switch (type)
	{
	case ColorType::AMBIENT:
		material->Get(AI_MATKEY_COLOR_AMBIENT, ret);
		our_mat->Ka.r = ret.r;
		our_mat->Ka.g = ret.g;
		our_mat->Ka.b = ret.b;
		break;
	case ColorType::DIFFUSE:
		material->Get(AI_MATKEY_COLOR_DIFFUSE, ret);
		our_mat->Kd.r = ret.r;
		our_mat->Kd.g = ret.g;
		our_mat->Kd.b = ret.b;
		break;
	case ColorType::SPECULAR:
		material->Get(AI_MATKEY_COLOR_SPECULAR, ret);
		our_mat->Ks.r = ret.r;
		our_mat->Ks.g = ret.g;
		our_mat->Ks.b = ret.b;
		break;
	default:
		std::cout << "Unknown Type\n";
		assert(0);
		break;
	}
	
}

void Model::ComputeNormal(std::vector<Vertex>& v, const std::vector<uint32_t>& idx)
{
	for (int i = 0; i < idx.size(); i += 3) {
		auto& v0 = v[idx[i + 0]];
		auto& v1 = v[idx[i + 1]];
		auto& v2 = v[idx[i + 2]];

		glm::vec3 n = glm::cross(v1.position - v0.position, v2.position - v0.position);
		
		v0.normal += n;  //when vertex is shared, average this attr
		v1.normal += n;
		v2.normal += n;
	}
	for (auto& x : v)
		x.normal = glm::normalize(x.normal);
		
}

void Model::ComputeTangent(std::vector<Vertex>& v, const std::vector<uint32_t>& idx)
{
// reference
// https://learnopengl-cn.github.io/05%20Advanced%20Lighting/04%20Normal%20Mapping/
// https://github.com/gongminmin/GoldenSun/blob/main/Source/DevHelper/Source/MeshHelper.cpp

	for (int i = 0; i < idx.size(); i += 3) {
		auto& v0 = v[idx[i + 0]];
		auto& v1 = v[idx[i + 1]];
		auto& v2 = v[idx[i + 2]];

		glm::vec3 e1 = v1.position - v0.position;
		glm::vec3 e2 = v2.position - v0.position;

		glm::vec2 dudv1 = v1.texcoord - v0.texcoord;
		glm::vec2 dudv2 = v2.texcoord - v0.texcoord;

		glm::vec3 tangent, bitangent;

		float D = dudv1.x * dudv2.y - dudv2.x * dudv1.y;
		if (std::abs(D) < std::numeric_limits<float>::epsilon())
		{
			tangent = glm::vec3(1, 0, 0);
			bitangent = glm::vec3(0, 1, 0);
		}
		else
		{
			tangent = (dudv2.y * e1 - dudv1.y * e2) / D;
			bitangent = (dudv1.x * e1 - dudv2.x * e2) / D;
		}

		v0.tangent += tangent;
		v0.bitangent += bitangent;
		v1.tangent += tangent;
		v1.bitangent += bitangent;
		v2.tangent += tangent;
		v2.bitangent += bitangent;
	}

	for (auto& x : v) {

		// Gram-Schmidt orthogonalize
		x.tangent = glm::normalize(x.tangent - glm::dot(x.tangent, x.normal) * x.normal);

		glm::vec3 bitangent = glm::cross(x.normal, x.tangent);
		// make sure right-handed coordinate
		if (glm::dot(bitangent, x.bitangent) < 0.0f) {
			bitangent = -bitangent;
		}
		x.bitangent = bitangent;
	}
}

Model& Model::BindModelMat(const glm::mat4& mat)
{
	model_matrix = mat;
	return *this;
}

Model& Model::BindModelTex(const char* path)
{
	std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(path);
	//tex->smode = Texture2D::SampleMode::Bilinear;
	std::shared_ptr<Material> mat = std::make_shared<Material>();
	mat->diffuse = tex;
	for (auto& mesh : meshes)
		mesh.material_idx = 0;
	materials[0] = mat;
	return *this;
}

void Model::LoadFromFile(const std::string& obj_path)
{
	Assimp::Importer imp;
	auto model = imp.ReadFile(obj_path,
		aiProcess_ValidateDataStructure |
		aiProcess_OptimizeMeshes |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
		//aiProcess_CalcTangentSpace   // 计算tangent\bitangent
		//aiProcess_GenNormals				// 如果读的文件里没有法线就自动生成
	);

	// check for errors
	if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode) 
	{
		std::cout << "load model error: " << obj_path << '\n';
		std::cout << "ERROR::ASSIMP: " << imp.GetErrorString() << '\n';
		return;
	}
	
	dir = obj_path.substr(0, obj_path.find_last_of('/'));
	
	// 其实所有数据都可以直接从scene读了，但一般为了保持模型的层次，一个node一个node处理
	ParseNode(model->mRootNode, model);

	
	size_t tot = 0;
	for (size_t i = 0; i < meshes.size(); ++i)
		tot += meshes[i].vertices.size();
	std::cout << "model load!\n" << tot << " vertices in total\n";

	assert(model->HasMaterials() == true);
	ParseMaterial(model);

	std::cout << obj_path << " material load " << textures.size() << " textures\n";
}
