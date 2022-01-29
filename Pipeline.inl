#include "Pipeline.h"

#include <GLFW/glfw3.h>

#include <algorithm>


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
		//std::cout << v0.proj_pos.x << ' ' << v0.proj_pos.y << ' ' << v0.proj_pos.z << ' ' << v0.proj_pos.w << '\n';

		// ��������mvp��ֱ���޳�������������Ļ�ռ�
		if (config.draw_mode == ConfigParams::DrawMode::WireFrame
		|| config.fc_order == ConfigParams::FaceCullOrder::NONE
		|| (config.fc_order == ConfigParams::FaceCullOrder::CCW && FaceCullCCW(v0.proj_pos, v1.proj_pos, v2.proj_pos))
		|| (config.fc_order == ConfigParams::FaceCullOrder::CW && FaceCullCW(v0.proj_pos, v1.proj_pos, v2.proj_pos))
		) {
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

	if (config.draw_mode == ConfigParams::DrawMode::WireFrame) {
		static const glm::vec4 color(1.0f);
		DrawLine(vv0.proj_pos.x, vv0.proj_pos.y, vv1.proj_pos.x, vv1.proj_pos.y, color);
		DrawLine(vv1.proj_pos.x, vv1.proj_pos.y, vv2.proj_pos.x, vv2.proj_pos.y, color);
		DrawLine(vv2.proj_pos.x, vv2.proj_pos.y, vv0.proj_pos.x, vv0.proj_pos.y, color);
	}
	else {
		RasterizeTriangle(vv0, vv1, vv2);
	}
	
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
	maxx = std::min(pContext->GetRenderTarget()->GetWidth() - 1, maxx + 1);
	miny = std::max(0, miny - 1);
	maxy = std::min(pContext->GetRenderTarget()->GetHeight() - 1, maxy + 1);

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
				if ( pContext->GetDepthBufferPointer()->TryUpdate(i, j, Z) ) {
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
					pContext->GetRenderTarget()->write(i, j, shader.ps(v2f, current_model_id, current_mesh_id));
				}
			}
		}
	}
}


// ����������������ķ���ֵд�����ɲμ�C++ Primer����������ģ�P593 -- ʹ��������ͳ�Ա
// �ڱ����ڣ�����������֪�� T::sth�Ǹ����� ������ T�е�ĳ��static��Ա
// ���� ��ʽtypename��Ŀ�ľ��Ǹ��߱����� T::sth �Ǹ����� ������ T�е�ĳ��static��Ա ��
template<class Shader>
inline typename Pipeline<Shader>::VSOut    // ����������͵�д���б�Ҫ�����������������
Pipeline<Shader>::DivideAndTransform(const VSOut& v)
{
	VSOut ret = v;

	float invW = 1.0f / ret.proj_pos.w;
	ret.proj_pos *= invW;  // ��shader�ж���VSOutҪ������Ҫ����*=����Ҳ������������λ���

	ret.proj_pos.x = (1.0f + ret.proj_pos.x) * pContext->GetRenderTarget()->GetWidth() * 0.5f;
	ret.proj_pos.y = (1.0f - ret.proj_pos.y) * pContext->GetRenderTarget()->GetHeight() * 0.5f;
	//  -1 <= z <= 1 
	ret.proj_pos.z = (1.0f + ret.proj_pos.z) * 0.5; // 0 <= z <= 1;

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
	eye = shader.vs.proj * eye;	// ��ͶӰ�任  ����perspective���󣬱任��eye.z < 0 
	//std::cout << eye.x << ' ' << eye.y << ' ' << eye.z << ' ' << eye.w << '\n';
	glm::vec4 cross = Cross(v1 - v0, v2 - v0);
	float res = glm::dot(cross, v0 - eye);
	return res >= 0.0f;
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
inline void Pipeline<Shader>::DrawMesh(const Mesh& mesh)
{	
	ProcessVertices(mesh.vertices, mesh.indices);
}

template<class Shader>
inline void Pipeline<Shader>::Draw(uint32_t model_id)
{
	//shader.vs.BindAllMat(
	//	pContext->models[model_id]->model_matrix,
	//	pContext->camera->GetView(),
	//	pContext->camera->GetProj()
	//);

	current_model_id = model_id;
	const auto& model = pContext->models[model_id];
	for (size_t i = 0; i < model->meshes.size(); ++i) {
		current_mesh_id = i;
		DrawMesh(model->meshes[i]);
	}	
}

template<class Shader>
inline void Pipeline<Shader>::BindConfig(const PipelineConfig& cfg)
{
	config = cfg;
}

template<class Shader>
inline void Pipeline<Shader>::BindContext(std::shared_ptr<SceneContext> context)
{
	shader.ps.pContext = context;
	pContext = context;
}

template<class Shader>
inline Shader& Pipeline<Shader>::GetShader()
{
	return shader;
}

template<class Shader>
inline void Pipeline<Shader>::DrawLine(const int x0, const int y0, const int x1, const int y1, const glm::vec4& color)
{

	int dx = x1 - x0;
	int dy = y1 - y0;
	int dLong = abs(dx);
	int dShort = abs(dy);
	int offsetLong = dx > 0 ? 1 : -1;
	int offsetShort = dy > 0 ? 1 : -1;
	bool f = true;
	if (dLong < dShort)
	{
		f = false;
		std::swap(dShort, dLong);
		std::swap(offsetShort, offsetLong);
	}
	int error = dShort - dLong / 2;
	int nx = x0, ny = y0;
	const int offset[] = { 0, offsetShort };
	const int abs_d[] = { dShort, dShort - dLong };
	if (f) {
		// ��ʵ�ĵ������������ôд�����ƺ������ڵ�д����һЩ
		//	(f ? nx : ny) += offsetLong;
		//	(f ? ny : nx) += offset[errorIsTooBig];
		for (int i = 0; i <= dLong; ++i)
		{
			pContext->GetRenderTarget()->write(nx, ny, color);
			const int errorIsTooBig = error >= 0;
			nx += offsetLong;
			ny += offset[errorIsTooBig];
			error += abs_d[errorIsTooBig];
		}
	}
	else {
		for (int i = 0; i <= dLong; ++i)
		{
			pContext->GetRenderTarget()->write(nx, ny, color);
			const int errorIsTooBig = error >= 0;
			ny += offsetLong;
			nx += offset[errorIsTooBig];
			error += abs_d[errorIsTooBig];
		}
	}
}