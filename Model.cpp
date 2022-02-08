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

	// vertices && normal && texcoord
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		Vertex v;
		v.position.x = mesh->mVertices[i].x;
		v.position.y = mesh->mVertices[i].y;
		v.position.z = mesh->mVertices[i].z;

		v.normal.x = mesh->mNormals[i].x;
		v.normal.y = mesh->mNormals[i].y;
		v.normal.z = mesh->mNormals[i].z;

		if (mesh->HasTextureCoords(0)) {
			v.texcoord.x = mesh->mTextureCoords[0][i].x;
			v.texcoord.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			v.texcoord = glm::vec2(0);
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

Model& Model::BindModelMat(const glm::mat4& mat)
{
	model_matrix = mat;
	return *this;
}

Model& Model::BindModelTex(const char* path)
{
	std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(path);
	tex->smode = Texture2D::SampleMode::Bilinear;
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
		aiProcess_OptimizeMeshes |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals				// 如果读的文件里没有法线就自动生成
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
