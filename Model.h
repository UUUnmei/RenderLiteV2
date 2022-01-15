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
	std::vector<Mesh> meshes;  // Mesh����material index��material����texture
	std::unordered_map<int, std::shared_ptr<Material>> materials;  // mat idx --> mat 
private:
	std::unordered_map<std::string, Texture2D> textures;	 // tex path --> tex �����ظ����أ������ʵֻ��һ�����ظ������ã�maybe����ĳ����ʽ��weak ptr
	

	std::string dir; // model directory 
	void ParseNode(aiNode* node, const aiScene* scene);
	void ParseMesh(aiMesh* mesh, const aiScene* scene);
	void ParseMaterial(const aiScene* scene);
	void LoadTypeTexture(std::shared_ptr<Material> our_mat, aiTextureType type, aiMaterial* material);
	void LoadParams(std::shared_ptr<Material> our_mat, ColorType type, aiMaterial* material);
public:
	Model& BindModelMat(const glm::mat4& mat);
	// �������ģ������mesh�������tex����ʱֻ����diffuse
	// һ���̶���ֻ��Ϊ�˷����ֶ�����
	// ��Ҫ����Ӧ�ÿ�ģ���ļ�������趨��Ӧ����ʹ��
	Model& BindModelTex(const char* path); 
	void LoadFromFile(const std::string& obj_path);



};

