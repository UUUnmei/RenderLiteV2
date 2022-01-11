#pragma once

#include "DepthBuffer.h"
#include "FrameBuffer.h"
#include "Mesh.h"


#include <memory>
#include <vector>

class SceneContext;


//关于为什么这里用模板，直接原因是为了在pipeline中
//想要根据需要使用相应的VSOut类型，
//而不是弄一个什么attr都有的巨大的顶点类型。
//但代价就是首先只能写.h文件了（+ .inl)
//其次如果做Shader实时切换的效果，
//那么每个Shader都对应一个pipeline类
//编译代价是比较大的
// 在构造scene对象的时候需要为pipeline指定shader
template<class Shader>
class Pipeline
{

	using VSOut = typename Shader::VSOut;  //typedef typename Shader::VSOut VSOut;
	
private:
	std::unique_ptr<DepthBuffer> depthbuffer;
	std::unique_ptr<FrameBuffer> framebuffer;
	Shader shader;
public:

	Pipeline(uint32_t w, uint32_t h);
	const unsigned char* GetRawFrameBufferPointer() const;

private:
	// 调用vertex shader
	void ProcessVertices(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	// 每三个顶点组合为一个三角形
	// + Face Cull
	void AssembleTriangles(const std::vector<VSOut>& vertices, const std::vector<uint32_t>& indices);
	// 做各种处理/测试
	// + Clip Cull
	// + Homogeneous Clip
	void ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2);
	// 转换到屏幕空间
	// + 透视除法
	void PostProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2);
	// 三角形光栅化，调用pixel shader
	void RasterizeTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2);


	// 透视除法，并变换到到屏幕空间
	VSOut DivideAndTransform(const VSOut& v);
	// 背面剔除，认为逆时针为正面  为真表示不剔除
	bool FaceCullCCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
	// 背面剔除，认为顺时针为正面  为真表示不剔除
	bool FaceCullCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
	// cvv内外判断  为真表示完全在cvv内
	bool CVVCheck(const glm::vec4& v);
	// TODO 齐次裁剪

	int current_model_id;
	int current_mesh_id;
public:
	void BeginFrame();
	void DrawMesh(const Mesh& mesh, int modelId, int meshId);
	void BindContext(std::shared_ptr<SceneContext> context);
	void BindModelMat(const glm::mat4& mat);
};


#include "Pipeline.inl"