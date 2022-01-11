#pragma once 

#include "Model.h"
#include "OrbitCamera.h"

#include <memory>
#include <vector>

class SceneContext
{
	

public: 
	std::shared_ptr<OrbitCamera> camera;
	std::vector<std::unique_ptr<Model>> models;

public:
	SceneContext();
	Model& AddModel(const std::string& path);
	void AddCamera(std::shared_ptr<OrbitCamera> camera);
	
};

