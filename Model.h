#pragma once

#include "Mesh.h"
#include "Texture2D.h"
#include "Material.h"

struct aiNode;
struct aiScene;
struct aiMesh;
#include <assimp/material.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class Model
{
public:
	glm::mat4 model_matrix = glm::mat4(1.0f);
	std::vector<Mesh> meshes;  // Mesh里有material index，material里有texture
	std::unordered_map<int, std::shared_ptr<Material>> materials;  // mat idx --> mat 
private:
	std::unordered_map<std::string, Texture2D> textures;	 // tex path --> tex 避免重复加载，这个其实只有一个防重复的作用，maybe换成某种形式的weak ptr
	

	std::string dir; // model directory 
	void ParseNode(aiNode* node, const aiScene* scene);
	void ParseMesh(aiMesh* mesh, const aiScene* scene);
	void ParseMaterial(const aiScene* scene);
	void LoadTypeTexture(std::shared_ptr<Material> our_mat, aiTextureType type, aiMaterial* material);
	void LoadParams(std::shared_ptr<Material> our_mat, ColorType type, aiMaterial* material);
public:
	Model& BindModelMat(const glm::mat4& mat);
	// 对于这个模型所有mesh都用这个tex，暂时只考虑diffuse
	// 一定程度上只是为了方便手动设置
	// 主要还是应该靠模型文件本身的设定，应避免使用
	Model& BindModelTex(const char* path); 
	void LoadFromFile(const std::string& obj_path);



};

