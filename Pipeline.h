#pragma once

#include "DepthBuffer.h"
#include "FrameBuffer.h"
#include "Mesh.h"


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
	std::unique_ptr<DepthBuffer> depthbuffer;
	std::unique_ptr<FrameBuffer> framebuffer;
	Shader shader;
public:

	Pipeline(uint32_t w, uint32_t h);
	const unsigned char* GetRawFrameBufferPointer() const;

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


	// ͸�ӳ��������任������Ļ�ռ�
	VSOut DivideAndTransform(const VSOut& v);
	// �����޳�����Ϊ��ʱ��Ϊ����  Ϊ���ʾ���޳�
	bool FaceCullCCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
	// �����޳�����Ϊ˳ʱ��Ϊ����  Ϊ���ʾ���޳�
	bool FaceCullCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
	// cvv�����ж�  Ϊ���ʾ��ȫ��cvv��
	bool CVVCheck(const glm::vec4& v);
	// TODO ��βü�

	int current_model_id;
	int current_mesh_id;
public:
	void BeginFrame();
	void DrawMesh(const Mesh& mesh, int modelId, int meshId);
	void BindContext(std::shared_ptr<SceneContext> context);
	void BindModelMat(const glm::mat4& mat);
};


#include "Pipeline.inl"