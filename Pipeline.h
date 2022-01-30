#pragma once

#include "Mesh.h"
#include "PipelineConfig.h"

#include <memory>
#include <vector>

class SceneContext;


//����Ϊʲô������ģ�壬ֱ��ԭ����Ϊ����pipeline��
//��Ҫ������Ҫʹ����Ӧ��VSOut���ͣ�
//������Ūһ��ʲôattr���еľ޴�Ķ������͡�
//�����۾�������ֻ��д.h�ļ��ˣ�+ .inl)
//��������Shaderʵʱ�л���Ч����
//��ôÿ��Shader����Ӧһ��pipeline��
//��������ǱȽϴ��
// �ڹ���scene�����ʱ����ҪΪpipelineָ��shader
template<class Shader>
class Pipeline
{

	using VSOut = typename Shader::VSOut;  //typedef typename Shader::VSOut VSOut;
private:
	Shader shader;
	std::shared_ptr<SceneContext> pContext{ nullptr };
public:
	PipelineConfig config;

	Pipeline() = default;
	

private:
	// ����vertex shader
	void ProcessVertices(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	// ÿ�����������Ϊһ��������
	// + Face Cull
	void AssembleTriangles(const std::vector<VSOut>& vertices, const std::vector<uint32_t>& indices);
	// �����ִ���/����
	// + Clip Cull
	// + Homogeneous Clip
	void ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2);
	// ת������Ļ�ռ�
	// + ͸�ӳ���
	void PostProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2);
	// �����ι�դ��������pixel shader
	void RasterizeTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2);
	void DrawLine(const int x0, const int y0, const int x1, const int y1, const glm::vec4& color);


	// ͸�ӳ��������任������Ļ�ռ�
	VSOut DivideAndTransform(const VSOut& v);
	// �����޳�����Ϊ��ʱ��Ϊ����  Ϊ���ʾ���޳�
	bool FaceCullCCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
	// �����޳�����Ϊ˳ʱ��Ϊ����  Ϊ���ʾ���޳�
	bool FaceCullCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
	// cvv�����ж�  Ϊ���ʾ��ȫ��cvv��
	bool CVVCheck(const glm::vec4& v);

	uint32_t current_model_id{ 0 };
	uint32_t current_mesh_id{ 0 };
public:
	void DrawMesh(const Mesh& mesh);
	void Draw(uint32_t model_id);

	void BindContext(std::shared_ptr<SceneContext> context);
	void BindConfig(const PipelineConfig& cfg);

	Shader& GetShader();
};


#include "Pipeline.inl"