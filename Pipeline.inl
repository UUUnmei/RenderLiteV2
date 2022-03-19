#include "Pipeline.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>


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

		// delay the backface culling until rasterize
		ProcessTriangle(v0, v1, v2);
	}
}

namespace {
	const std::vector<glm::vec4> clip_planes{
		{0,0,1,1},  //near MUST 
		{0,0,-1,1}, //far
		{1,0,0,1}, //left
		{0,1,0,1},  //top
		{-1,0,0,1}, //right
		{0,-1,0,1}  //bottom 
	};

	void print(const glm::vec4& v) {
		std::cout << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
	}
	void println(const glm::vec4& v) {
		print(v);
		std::cout << '\n';
	}

	// v0v1 counter-clock-wise
	bool IsTopLeft(const glm::ivec2& v0, const glm::ivec2& v1) {
		bool valid = (v0.y < v1.y)  // left(Y increase from top to bottom in screen space)
			|| (v0.y == v1.y && v0.x > v1.x);  // top
		return valid;
	}

	struct Vec2i{
		glm::ivec4 x, y;
	};

}


template<class Shader>
inline void Pipeline<Shader>::ProcessTriangle(const VSOut& v0, const VSOut& v1, const VSOut& v2)
{
	//println(v0.proj_pos);
	//println(v1.proj_pos);
	//println(v2.proj_pos);

	// 默认使用右手系、x,y,z的合法范围都是[-w,+w]
//#define SIMPLE_CLIP
#ifdef SIMPLE_CLIP
	if (!CVVCheck(v0.proj_pos) || !CVVCheck(v1.proj_pos) || !CVVCheck(v2.proj_pos))
		return;
	PostProcessTriangle(v0, v1, v2);
#else
	// discard points totally outside
	if (v0.proj_pos.x > v0.proj_pos.w && v1.proj_pos.x > v1.proj_pos.w && v2.proj_pos.x > v2.proj_pos.w) return;
	if (v0.proj_pos.x < -v0.proj_pos.w && v1.proj_pos.x < -v1.proj_pos.w && v2.proj_pos.x < -v2.proj_pos.w) return;
	if (v0.proj_pos.y > v0.proj_pos.w && v1.proj_pos.y > v1.proj_pos.w && v2.proj_pos.y > v2.proj_pos.w) return;
	if (v0.proj_pos.y < -v0.proj_pos.w && v1.proj_pos.y < -v1.proj_pos.w && v2.proj_pos.y < -v2.proj_pos.w) return;
	if (v0.proj_pos.z > v0.proj_pos.w && v1.proj_pos.z > v1.proj_pos.w && v2.proj_pos.z > v2.proj_pos.w) return;
	if (v0.proj_pos.z < -v0.proj_pos.w && v1.proj_pos.z < -v1.proj_pos.w && v2.proj_pos.z < -v2.proj_pos.w) return;
	// if totally inside, no clip
	if (CVVCheck(v0.proj_pos) && CVVCheck(v1.proj_pos) && CVVCheck(v2.proj_pos)) 
		PostProcessTriangle(v0, v1, v2);

	// homogeneous clipping
	// Sutherland_Hodgeman

	auto get_intersection = [&](int plane, const VSOut& p1, const VSOut& p2, VSOut& res)
		-> void {
		float d1 = glm::dot(clip_planes[plane], p1.proj_pos);
		float d2 = glm::dot(clip_planes[plane], p2.proj_pos);
		float t = d1 / (d1 - d2);
		res = p1 * (1 - t) + p2 * t;  // 需要重载+*
	};
	auto inside_test = [&](int plane, const glm::vec4& point) -> bool {
		return glm::dot(clip_planes[plane], point) >= 0;  // weired float precision when testing skybox 
		//suppose near plane
		//same as z+w>0 => z>-w
	};

	VSOut vso[10], tmp[10]; 
	int len = 3;
	vso[0] = v0, vso[1] = v1, vso[2] = v2;
	for (int k = 0; k < clip_planes.size(); ++k) {
		int cnt = 0;
		for (int i = 0; i < len; ++i) {
			int j = (i + 1) % len;

			bool f1 = inside_test(k, vso[i].proj_pos); //为真表示该点在要保留的范围
			bool f2 = inside_test(k, vso[j].proj_pos);

			if (f1 && f2) {
				// 两点都可见 要第二个点
				tmp[cnt++] = vso[j];
			}
			else if (f1) {
				// 只有第一个点可见， 要与边界的交点
				get_intersection(k, vso[i], vso[j], tmp[cnt++]);
			}
			else if (f2) {
				// 只有第二个点可见，要交点和第二个点
				get_intersection(k, vso[i], vso[j], tmp[cnt++]);
				tmp[cnt++] = vso[j];
			}
			else {
				// 都不可见，弃之
			}
		}
		assert(cnt <= 7);
		len = cnt;
		for (int i = 0; i < cnt; ++i)
			vso[i] = tmp[i];
	}
	
	// process cliped triangles
	for (int i = 0; i < len - 2; ++i) {
		PostProcessTriangle(
			vso[0],
			vso[i + 1],
			vso[i + 2]
		);
	}
#endif
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
#if 1
	// 2*2

	// AABB
	glm::ivec2 vv0(v0.proj_pos.x + 0.5f, v0.proj_pos.y + 0.5f);
	glm::ivec2 vv1(v1.proj_pos.x + 0.5f, v1.proj_pos.y + 0.5f);
	glm::ivec2 vv2(v2.proj_pos.x + 0.5f, v2.proj_pos.y + 0.5f);

	int minx = std::min({ vv0.x, vv1.x, vv2.x });
	int maxx = std::max({ vv0.x, vv1.x, vv2.x });
	int miny = std::min({ vv0.y, vv1.y, vv2.y });
	int maxy = std::max({ vv0.y, vv1.y, vv2.y });

	minx = std::max(0, minx);
	maxx = std::min(pContext->GetRenderTarget()->GetWidth() - 1, maxx);
	miny = std::max(0, miny);
	maxy = std::min(pContext->GetRenderTarget()->GetHeight() - 1, maxy);

	// start process triangle
	int dy01 = -(vv1.y - vv0.y), dx01 = vv1.x - vv0.x;
	int dy12 = -(vv2.y - vv1.y), dx12 = vv2.x - vv1.x;
	int dy20 = -(vv0.y - vv2.y), dx20 = vv0.x - vv2.x;

	int det = dx01 * dy20 - dx20 * dy01;
	if (det == 0) return;
	// this renderer actually treats CCW as front face by default
	// but this orientation produces negative cross product in screen space
	// and here want to keep consistent with normal Cartesian coordinate
	// so lets flip it
	/// backface culling here 
	/// more accurate while sacrificing a bit of performance than doing it just after MVP
	if (config.fc_order == ConfigParams::FaceCullOrder::CCW && det > 0) return;
	if (config.fc_order == ConfigParams::FaceCullOrder::CW && det < 0) return;
	if (det < 0) {
		det *= -1;
		dx01 *= -1;
		dy01 *= -1;
		dx12 *= -1;
		dy12 *= -1;
		dx20 *= -1;
		dy20 *= -1;
	}
	float _inv = 1.0f / det;

	glm::ivec4 DY01(dy01), DY12(dy12), DY20(dy20);
	glm::ivec4 DX01(dx01), DX12(dx12), DX20(dx20);

	Vec2i V0{ glm::ivec4(vv0.x), glm::ivec4(vv0.y) };
	Vec2i V1{ glm::ivec4(vv1.x), glm::ivec4(vv1.y) };
	Vec2i V2{ glm::ivec4(vv2.x), glm::ivec4(vv2.y) };

	minx -= minx % 2;
	miny -= miny % 2;
	// 2 * 2 quad
	Vec2i P{ minx + glm::ivec4(0, 1, 0, 1), miny + glm::ivec4(0, 0, 1, 1) };

	int bias0 = IsTopLeft(vv0, vv1) ? 0 : -1;
	int bias1 = IsTopLeft(vv1, vv2) ? 0 : -1;
	int bias2 = IsTopLeft(vv2, vv0) ? 0 : -1;
	glm::ivec4 BIAS0(bias0);
	glm::ivec4 BIAS1(bias1);
	glm::ivec4 BIAS2(bias2);

	glm::ivec4 _w0 = DX12 * (P.y - V1.y) + DY12 * (P.x - V1.x) + BIAS1;
	glm::ivec4 _w1 = DX20 * (P.y - V2.y) + DY20 * (P.x - V2.x) + BIAS2;
	glm::ivec4 _w2 = DX01 * (P.y - V0.y) + DY01 * (P.x - V0.x) + BIAS0;

	for (int y = miny; y <= maxy; y += 2) {
		glm::ivec4 w0 = _w0;
		glm::ivec4 w1 = _w1;
		glm::ivec4 w2 = _w2;
		for (int x = minx; x <= maxx; x += 2) {

			// if(w0 >= 0 && w1 >= 0 && w2 >= 0) { 
			if ((w0.x | w1.x | w2.x) >= 0) { // take advantage of sign bit in complement representation
				float lambda0 = w0.x * _inv;
				float lambda1 = w1.x * _inv;
				//float lambda2 = w2.x * _inv; // not preferred
				float lambda2 = 1.0f - lambda0 - lambda1;
			
				VSOut v2f;
				//v2f.proj_pos.x = P.x + 0.5f;	// gl_fragCoord
				//v2f.proj_pos.y = P.y + 0.5f;
				v2f.proj_pos.z = v0.proj_pos.z * lambda0 + v1.proj_pos.z * lambda1 + v2.proj_pos.z * lambda2;
				//v2f.proj_pos.w = v0.proj_pos.w * lambda0.x + v1.proj_pos.w * lambda1.x + v2.proj_pos.w * lambda2.x;

				if (pContext->GetDepthBufferPointer()->TryUpdate(x, y, v2f.proj_pos.z)) {
					float b0 = lambda0 * v0.proj_pos.w;
					float b1 = lambda1 * v1.proj_pos.w;
					float b2 = lambda2 * v2.proj_pos.w;
					float inv = 1.0f / (b0 + b1 + b2);
					b0 *= inv;
					b1 *= inv;
					//b2 *= inv; // seems ok
					b2 = 1 - b0 - b1;
					//std::cout << b0 << ' ' << b1 << ' ' << b2 << '\n';

					v2f.Lerp(v0, v1, v2, b0, b1, b2);

					pContext->GetRenderTarget()->write(x, y, shader.ps(v2f, current_model_id, current_mesh_id));

				} // if nearer
			} // if inside

			if ((w0.y | w1.y | w2.y) >= 0) { // take advantage of sign bit in complement representation
				float lambda0 = w0.y * _inv;
				float lambda1 = w1.y * _inv;
				//float lambda2 = w2.x * _inv; // not preferred
				float lambda2 = 1.0f - lambda0 - lambda1;

				VSOut v2f;
				//v2f.proj_pos.x = P.x + 0.5f;	// gl_fragCoord
				//v2f.proj_pos.y = P.y + 0.5f;
				v2f.proj_pos.z = v0.proj_pos.z * lambda0 + v1.proj_pos.z * lambda1 + v2.proj_pos.z * lambda2;
				//v2f.proj_pos.w = v0.proj_pos.w * lambda0.x + v1.proj_pos.w * lambda1.x + v2.proj_pos.w * lambda2.x;

				if (pContext->GetDepthBufferPointer()->TryUpdate(x + 1, y, v2f.proj_pos.z)) {
					float b0 = lambda0 * v0.proj_pos.w;
					float b1 = lambda1 * v1.proj_pos.w;
					float b2 = lambda2 * v2.proj_pos.w;
					float inv = 1.0f / (b0 + b1 + b2);
					b0 *= inv;
					b1 *= inv;
					//b2 *= inv; // seems ok
					b2 = 1 - b0 - b1;
					//std::cout << b0 << ' ' << b1 << ' ' << b2 << '\n';

					v2f.Lerp(v0, v1, v2, b0, b1, b2);

					pContext->GetRenderTarget()->write(x + 1, y, shader.ps(v2f, current_model_id, current_mesh_id));

				} // if nearer
			} // if inside

			if ((w0.z | w1.z | w2.z) >= 0) { // take advantage of sign bit in complement representation
				float lambda0 = w0.z * _inv;
				float lambda1 = w1.z * _inv;
				//float lambda2 = w2.x * _inv; // not preferred
				float lambda2 = 1.0f - lambda0 - lambda1;

				VSOut v2f;
				//v2f.proj_pos.x = P.x + 0.5f;	// gl_fragCoord
				//v2f.proj_pos.y = P.y + 0.5f;
				v2f.proj_pos.z = v0.proj_pos.z * lambda0 + v1.proj_pos.z * lambda1 + v2.proj_pos.z * lambda2;
				//v2f.proj_pos.w = v0.proj_pos.w * lambda0.x + v1.proj_pos.w * lambda1.x + v2.proj_pos.w * lambda2.x;

				if (pContext->GetDepthBufferPointer()->TryUpdate(x, y + 1, v2f.proj_pos.z)) {
					float b0 = lambda0 * v0.proj_pos.w;
					float b1 = lambda1 * v1.proj_pos.w;
					float b2 = lambda2 * v2.proj_pos.w;
					float inv = 1.0f / (b0 + b1 + b2);
					b0 *= inv;
					b1 *= inv;
					//b2 *= inv; // seems ok
					b2 = 1 - b0 - b1;
					//std::cout << b0 << ' ' << b1 << ' ' << b2 << '\n';

					v2f.Lerp(v0, v1, v2, b0, b1, b2);

					pContext->GetRenderTarget()->write(x, y + 1, shader.ps(v2f, current_model_id, current_mesh_id));

				} // if nearer
			} // if inside
			
			if ((w0.w | w1.w | w2.w) >= 0) { // take advantage of sign bit in complement representation
				float lambda0 = w0.w * _inv;
				float lambda1 = w1.w * _inv;
				//float lambda2 = w2.x * _inv; // not preferred
				float lambda2 = 1.0f - lambda0 - lambda1;

				VSOut v2f;
				//v2f.proj_pos.x = P.x + 0.5f;	// gl_fragCoord
				//v2f.proj_pos.y = P.y + 0.5f;
				v2f.proj_pos.z = v0.proj_pos.z * lambda0 + v1.proj_pos.z * lambda1 + v2.proj_pos.z * lambda2;
				//v2f.proj_pos.w = v0.proj_pos.w * lambda0.x + v1.proj_pos.w * lambda1.x + v2.proj_pos.w * lambda2.x;

				if (pContext->GetDepthBufferPointer()->TryUpdate(x + 1, y + 1, v2f.proj_pos.z)) {
					float b0 = lambda0 * v0.proj_pos.w;
					float b1 = lambda1 * v1.proj_pos.w;
					float b2 = lambda2 * v2.proj_pos.w;
					float inv = 1.0f / (b0 + b1 + b2);
					b0 *= inv;
					b1 *= inv;
					//b2 *= inv; // seems ok
					b2 = 1 - b0 - b1;
					//std::cout << b0 << ' ' << b1 << ' ' << b2 << '\n';

					v2f.Lerp(v0, v1, v2, b0, b1, b2);

					pContext->GetRenderTarget()->write(x + 1, y + 1, shader.ps(v2f, current_model_id, current_mesh_id));

				} // if nearer
			} // if inside


			w0 += DY12 * 2;
			w1 += DY20 * 2;
			w2 += DY01 * 2;
		} // for x
		_w0 += DX12 * 2;
		_w1 += DX20 * 2;
		_w2 += DX01 * 2;
	} // for y

#else
	// AABB
	glm::ivec2 vv0(v0.proj_pos.x + 0.5f, v0.proj_pos.y + 0.5f);
	glm::ivec2 vv1(v1.proj_pos.x + 0.5f, v1.proj_pos.y + 0.5f);
	glm::ivec2 vv2(v2.proj_pos.x + 0.5f, v2.proj_pos.y + 0.5f);

	int minx = std::min({ vv0.x, vv1.x, vv2.x });
	int maxx = std::max({ vv0.x, vv1.x, vv2.x });
	int miny = std::min({ vv0.y, vv1.y, vv2.y });
	int maxy = std::max({ vv0.y, vv1.y, vv2.y });

	minx = std::max(0, minx);
	maxx = std::min(pContext->GetRenderTarget()->GetWidth() - 1, maxx);
	miny = std::max(0, miny);
	maxy = std::min(pContext->GetRenderTarget()->GetHeight() - 1, maxy);

	// start process triangle
	int dy01 = -(vv1.y - vv0.y), dx01 = vv1.x - vv0.x;
	int dy12 = -(vv2.y - vv1.y), dx12 = vv2.x - vv1.x;
	int dy20 = -(vv0.y - vv2.y), dx20 = vv0.x - vv2.x;

	int det = dx01 * dy20 - dx20 * dy01;
	if (det == 0) return;
	// this renderer actually treats CCW as front face by default
	// but this orientation produces negative cross product in screen space
	// and here want to keep consistent with normal Cartesian coordinate
	// so lets flip it
	/// backface culling here 
	/// more accurate while sacrificing a bit of performance than doing it just after MVP
	if (config.fc_order == ConfigParams::FaceCullOrder::CCW && det > 0) return; 
	if (config.fc_order == ConfigParams::FaceCullOrder::CW && det < 0) return;
	if (det < 0) {
		det *= -1;
		dx01 *= -1;
		dy01 *= -1;
		dx12 *= -1;
		dy12 *= -1;
		dx20 *= -1;
		dy20 *= -1;
	}
	float _inv = 1.0f / det;

	glm::ivec2 P{ minx, miny };

	int bias0 = IsTopLeft(vv0, vv1) ? 0 : -1;
	int bias1 = IsTopLeft(vv1, vv2) ? 0 : -1;
	int bias2 = IsTopLeft(vv2, vv0) ? 0 : -1;
	// _w0 = cross(vv2 - vv1, P - vv1)
	int _w0 = dx12 * (P.y - vv1.y) + dy12 * (P.x - vv1.x) + bias1;
	// _w1 = cross(vv0 - vv2, P - vv2)
	int _w1 = dx20 * (P.y - vv2.y) + dy20 * (P.x - vv2.x) + bias2;
	// _w2 = cross(vv1 - vv0, P - vv0)
	int _w2 = dx01 * (P.y - vv0.y) + dy01 * (P.x - vv0.x) + bias0;


	for (P.y = miny; P.y <= maxy; ++P.y) {
		int w0 = _w0;
		int w1 = _w1;
		int w2 = _w2;
		for (P.x = minx; P.x <= maxx; ++P.x) {

			// if(w0 >= 0 && w1 >= 0 && w2 >= 0) { 
			if ((w0 | w1 | w2) >= 0) { // take advantage of sign bit in complement representation
				float lambda0 = w0 * _inv;
				float lambda1 = w1 * _inv;
				//float lambda2 = w2 * _inv; // not preferred
				float lambda2 = 1 - lambda0 - lambda1;
				//std::cout << lambda0 << ' ' << lambda1 << ' ' << lambda2 << '\n';

				VSOut v2f;
				v2f.proj_pos.x = P.x + 0.5f;	// gl_fragCoord
				v2f.proj_pos.y = P.y + 0.5f;
				v2f.proj_pos.z = v0.proj_pos.z * lambda0 + v1.proj_pos.z * lambda1 + v2.proj_pos.z * lambda2;
				v2f.proj_pos.w = v0.proj_pos.w * lambda0 + v1.proj_pos.w * lambda1 + v2.proj_pos.w * lambda2;
				
				if (pContext->GetDepthBufferPointer()->TryUpdate(P.x, P.y, v2f.proj_pos.z)) {
					float b0 = lambda0 * v0.proj_pos.w;
					float b1 = lambda1 * v1.proj_pos.w;
					float b2 = lambda2 * v2.proj_pos.w;
					float inv = 1.0f / (b0 + b1 + b2);
					b0 *= inv;
					b1 *= inv;
					//b2 *= inv; // seems ok
					b2 = 1 - b0 - b1;
					//std::cout << b0 << ' ' << b1 << ' ' << b2 << '\n';

					v2f.Lerp(v0, v1, v2, b0, b1, b2);

					pContext->GetRenderTarget()->write(P.x, P.y, shader.ps(v2f, current_model_id, current_mesh_id));
				
				} // if nearer

			} // if inside

			w0 += dy12;
			w1 += dy20;
			w2 += dy01;
		} // for x
		_w0 += dx12;
		_w1 += dx20;
		_w2 += dx01;
	} // for y
#endif
}


// 关于下面这个函数的返回值写法，可参见C++ Primer（第五版中文）P593 -- 使用类的类型成员
// 在编译期，编译器还不知道 T::sth是个类型 还是是 T中的某个static成员
// 所以 显式typename的目的就是告诉编译器 T::sth 是个类型 而不是 T中的某个static成员 ！
template<class Shader>
inline typename Pipeline<Shader>::VSOut    // 这个返回类型的写法有必要单拎出来看看。。。
Pipeline<Shader>::DivideAndTransform(const VSOut& v)
{
	VSOut ret = v;

	float invW = 1.0f / ret.proj_pos.w;
	ret.proj_pos *= invW; 
	ret.proj_pos.w = invW;

	ret.proj_pos.x = (1.0f + ret.proj_pos.x) * pContext->GetRenderTarget()->GetWidth() * 0.5f;
	ret.proj_pos.y = (1.0f - ret.proj_pos.y) * pContext->GetRenderTarget()->GetHeight() * 0.5f;
	//  -1 <= z <= 1 
	ret.proj_pos.z = (1.0f + ret.proj_pos.z) * 0.5; // 0 <= z <= 1;

	return ret;
}

// 对于四维向量叉乘
static inline glm::vec4 Cross(const glm::vec4& v0, const glm::vec4& v1) {
	glm::vec3 vv0 = glm::vec3(v0.x, v0.y, v0.z);
	glm::vec3 vv1 = glm::vec3(v1.x, v1.y, v1.z);
	glm::vec3 res = glm::cross(vv0, vv1);
	return glm::vec4(res, 0.0f);
}

template<class Shader>
inline bool Pipeline<Shader>::CVVCheck(const glm::vec4& v)
{
	// 默认使用右手系、x,y,z的合法范围都是[-w,+w]
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
		// 其实改的是这个，但这么写性能似乎比现在的写法慢一些
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