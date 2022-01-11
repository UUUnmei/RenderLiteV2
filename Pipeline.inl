#include "Pipeline.h"

#include <GLFW/glfw3.h>

#include <algorithm>

template<class Shader>
inline Pipeline<Shader>::Pipeline(uint32_t w, uint32_t h)
{
	framebuffer = std::make_unique<FrameBuffer>(w, h);
	depthbuffer = std::make_unique<DepthBuffer>(w, h);
}

template<class Shader>
inline const unsigned char* Pipeline<Shader>::GetRawFrameBufferPointer() const
{
	return framebuffer->Get();
}

template<class Shader>
inline void Pipeline<Shader>::ProcessVertices(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	std::vector<VSOut> verticesOut( vertices.size() );
	std::transform(vertices.cbegin(), vertices.cend(), verticesOut.begin(), shader.vs);
	AssembleTriangles(verticesOut, indices);
}

template<class Shader>
inline void Pipeline<Shader>::AssembleTriangles(const std::vector<VSOut>& vertices, const std::vector<uint32_t>& indices)
{

	for (size_t i = 0; i < indices.size(); i += 3) {
		const auto& v0 = vertices[indices[i + 0]];
		const auto& v1 = vertices[indices[i + 1]];
		const auto& v2 = vertices[indices[i + 2]];

		// ��������mvp��ֱ���޳�������������Ļ�ռ�
		if (FaceCullCW(v0.proj_pos, v1.proj_pos, v2.proj_pos)) {
			ProcessTriangle(v0, v1, v2);
		}
		
	}
}

template<class Shader>
inline void Pipeline<Shader>::ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2)
{
	// Ĭ��ʹ������ϵ��x,y,z�ĺϷ���Χ����[-w,+w]
	if (!CVVCheck(v0.proj_pos) || !CVVCheck(v1.proj_pos) || !CVVCheck(v2.proj_pos)) return;

	PostProcessTriangle(v0, v1, v2);
}

template<class Shader>
inline void Pipeline<Shader>::PostProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2)
{
	VSOut vv0 = DivideAndTransform(v0);
	VSOut vv1 = DivideAndTransform(v1);
	VSOut vv2 = DivideAndTransform(v2);

	RasterizeTriangle(vv0, vv1, vv2);
}


template<class Shader>
inline void Pipeline<Shader>::RasterizeTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2)
{
	glm::vec2 vv0(v0.proj_pos.x, v0.proj_pos.y);
	glm::vec2 vv1(v1.proj_pos.x, v1.proj_pos.y);
	glm::vec2 vv2(v2.proj_pos.x, v2.proj_pos.y);

	int minx = std::floor(std::min({ vv0.x, vv1.x, vv2.x }));
	int maxx = std::ceil(std::max({ vv0.x, vv1.x, vv2.x }));
	int miny = std::floor(std::min({ vv0.y, vv1.y, vv2.y }));
	int maxy = std::ceil(std::max({ vv0.y, vv1.y, vv2.y }));
	minx = std::max(0, minx - 1);
	maxx = std::min(framebuffer->GetWidth() - 1, maxx + 1);
	miny = std::max(0, miny);
	maxy = std::min(framebuffer->GetHeight() - 1, maxy + 1);

	// ����������� Barycentric
	// reference  https://zhuanlan.zhihu.com/p/337296743
	glm::mat2 interpTransform(vv1 - vv0, vv2 - vv0);
	interpTransform = glm::inverse(interpTransform);

	for (int j = miny; j <= maxy; ++j) {
		for (int i = minx; i <= maxx; ++i) {
			glm::vec2 current(i + 0.5f, j + 0.5f);
			current  = interpTransform * (current - vv0);
			glm::vec3 bary = glm::vec3(1 - current.x - current.y, current.x, current.y); // Barycentric
		
			// �����Բ�ֵ
			// reference https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf P501
			// reference ��Fundamentals of Computer Graphics, Fourth Edition�� P258

			if (bary.x >= 0 && bary.y >= 0 && bary.z >= 0) {  // ����������
				
				// ��Ȳ���Ҳ������1/w����z
				float Z = v0.proj_pos.z * bary.x + v1.proj_pos.z * bary.y + v2.proj_pos.z * bary.z;
				
				//��Ȳ���
				if ( depthbuffer->TryUpdate(i, j, Z) ) {
					bary.x *= v0.proj_pos.w;  // ע������w��1/w
					bary.y *= v1.proj_pos.w;
					bary.z *= v2.proj_pos.w;
					float inv = 1.0f / ( bary.x + bary.y + bary.z );
					bary *= inv;

					VSOut v2f;
					// ��ֵ���ָ㷨��
					// 1�� VSOut����* + ���ô���pipeline���ù�����Щ������Ҫ��ֵ�������Ƕ�������һ��Ӱ��
					//v2f = v0 * bary.x + v1 * bary.y + v2 * bary.z;
					// 2�� �ֶ�����Ҫ������дһ�飬�ô������ܽϺã���������Ҫ����shader�ģ���Ͳ�̫�ã��·�̶����ߡ���
					//v2f.normal = v0.normal * bary.x + v1.normal * bary.y + v2.normal * bary.z;
					//v2f.texcoord = v0.texcoord * bary.x + v1.texcoord * bary.y + v2.texcoord * bary.z;
					// 3�� ������Ŀǰ��������ȡ����һ��ƽ��
					v2f = VSOut::Lerp(v0, v1, v2, bary.x, bary.y, bary.z);
					// 4�� ���䣿��
					

					// ���DivideAndTransform����������Ҳ��w �������Ҫ�ָ����ڲ�ֵ��
					//const float w = 1.0f / v2f.proj_pos.w;   
					//v2f *= w;
					framebuffer->write(i, j, shader.ps(v2f, current_model_id, current_mesh_id));
				}
			}
		}
	}
}


// ����������������ķ���ֵд�����ɲμ�C++ Primer����������ģ�P593 -- ʹ��������ͳ�Ա
// �ڱ����ڣ������ڻ���֪�� T::sth�Ǹ����� ������ T�е�ĳ��static��Ա
// ���� ��ʽtypename��Ŀ�ľ��Ǹ��߱����� T::sth �Ǹ����� ������ T�е�ĳ��static��Ա ��
template<class Shader>
inline typename Pipeline<Shader>::VSOut    // ����������͵�д���б�Ҫ�����������������
Pipeline<Shader>::DivideAndTransform(const VSOut& v)
{
	VSOut ret = v;

	float invW = 1.0f / ret.proj_pos.w;
	ret.proj_pos *= invW;  // ��shader�ж���VSOutҪ������Ҫ����*=����Ҳ������������λ���

	ret.proj_pos.x = (1.0f + ret.proj_pos.x) * framebuffer->GetWidth() * 0.5f;
	ret.proj_pos.y = (1.0f - ret.proj_pos.y) * framebuffer->GetHeight() * 0.5f;
	//  -1 <= z <= 1 

	// �� 1/w ����w�������ڲ�ֵʱ�ָ�
	ret.proj_pos.w = invW;

	return ret;
}

// ������ά�������
static inline glm::vec4 Cross(const glm::vec4& v0, const glm::vec4& v1) {
	glm::vec3 vv0 = glm::vec3(v0.x, v0.y, v0.z);
	glm::vec3 vv1 = glm::vec3(v1.x, v1.y, v1.z);
	glm::vec3 res = glm::cross(vv0, vv1);
	return glm::vec4(res, 0.0f);
}

template<class Shader>
inline bool Pipeline<Shader>::FaceCullCCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2)
{
	glm::vec4 eye(0.0f, 0.0f, 0.0f, 1.0f);  // view�任��������ԭ��
	eye = shader.vs.proj * eye;	// ��ͶӰ�任
	glm::vec4 cross = Cross(v1 - v0, v2 - v0);
	float res = glm::dot(cross, v0 - eye);
	return res <= 0.0f;
}

template<class Shader>
inline bool Pipeline<Shader>::FaceCullCW(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2)
{
	return !FaceCullCCW(v0, v1, v2);
}

template<class Shader>
inline bool Pipeline<Shader>::CVVCheck(const glm::vec4& v)
{
	// Ĭ��ʹ������ϵ��x,y,z�ĺϷ���Χ����[-w,+w]
	if (v.x < -v.w) return false;
	if (v.x > v.w) return false;
	if (v.y < -v.w) return false;
	if (v.y > v.w) return false;
	if (v.z < -v.w) return false;
	if (v.z > v.w) return false;
	return true;
}

template<class Shader>
inline void Pipeline<Shader>::BeginFrame()
{
	static const glm::vec4 color(0.2f, 0.2f, 0.8f, 1.0f);
	framebuffer->Clear(color);
	depthbuffer->clear();
}

template<class Shader>
inline void Pipeline<Shader>::BindModelMat(const glm::mat4& mat)
{
	shader.vs.BindMatModel(mat);
}

template<class Shader>
inline void Pipeline<Shader>::DrawMesh(const Mesh& mesh, int modelId, int meshId)
{	
	current_model_id = modelId;
	current_mesh_id = meshId;
	ProcessVertices(mesh.vertices, mesh.indices);
}

template<class Shader>
inline void Pipeline<Shader>::BindContext(std::shared_ptr<SceneContext> context)
{
	shader.vs.BindMatView(context->camera->GetView());
	shader.vs.BindMatProj(context->camera->GetProj());
	shader.ps.pContext = context;
}

