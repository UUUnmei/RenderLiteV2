#include "Mesh.h"

#include <unordered_map>

Mesh::Mesh(const std::vector<Vertex>& v, const std::vector<uint32_t>& i, int material_idx)
	: vertices(v), indices(i), material_idx(material_idx) { }

Mesh Mesh::GenPlane(float scale)
{
	const float len = scale / 2;  // half edge length
	std::vector<Vertex> vertices(4);
	
	vertices[0].position = glm::vec3(len, len, 0.0f);
	vertices[1].position = glm::vec3(len, -len, 0.0f);
	vertices[2].position = glm::vec3(-len, len, 0.0f);
	vertices[3].position = glm::vec3(-len, -len, 0.0f);
	vertices[0].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[2].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[3].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[0].texcoord = glm::vec2(1.0f, 1.0f);
	vertices[1].texcoord = glm::vec2(1.0f, 0.0f);
	vertices[2].texcoord = glm::vec2(0.0f, 1.0f);
	vertices[3].texcoord = glm::vec2(0.0f, 0.0f);

	std::vector<uint32_t> indices{
		0, 2, 3,
		0, 3, 1
	};

	return Mesh(vertices, indices, 0);
}

Mesh Mesh::GenCube(float scale)
{
	const float len = scale / 2;  // half edge length
	std::vector<Vertex> vertices(24);

	// front
	vertices[0].position  = glm::vec3(len, len, len);
	vertices[1].position  = glm::vec3(len, -len, len);
	vertices[2].position  = glm::vec3(-len, len, len);
	vertices[3].position  = glm::vec3(-len, -len, len);
	vertices[0].normal    = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[1].normal    = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[2].normal    = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[3].normal    = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices[0].texcoord  = glm::vec2(1.0f, 1.0f);   // each face display a full texture
	vertices[1].texcoord  = glm::vec2(1.0f, 0.0f);
	vertices[2].texcoord  = glm::vec2(0.0f, 1.0f);
	vertices[3].texcoord  = glm::vec2(0.0f, 0.0f);
	// back				    
	vertices[4].position  = glm::vec3(len, len, -len);
	vertices[5].position  = glm::vec3(len, -len, -len);
	vertices[6].position  = glm::vec3(-len, len, -len);
	vertices[7].position  = glm::vec3(-len, -len, -len);
	vertices[4].normal	  = glm::vec3(0.0f, 0.0f, -1.0f);
	vertices[5].normal	  = glm::vec3(0.0f, 0.0f, -1.0f);
	vertices[6].normal	  = glm::vec3(0.0f, 0.0f, -1.0f);
	vertices[7].normal	  = glm::vec3(0.0f, 0.0f, -1.0f);
	vertices[4].texcoord  = glm::vec2(0.0f, 1.0f);
	vertices[5].texcoord  = glm::vec2(0.0f, 0.0f);
	vertices[6].texcoord  = glm::vec2(1.0f, 1.0f);
	vertices[7].texcoord  = glm::vec2(1.0f, 0.0f);
	// left
	vertices[8].position  =	glm::vec3(-len, len, len);
	vertices[9].position  =	glm::vec3(-len, -len, len);
	vertices[10].position =	glm::vec3(-len, len, -len);
	vertices[11].position =	glm::vec3(-len, -len, -len);
	vertices[8].normal    = glm::vec3(-1.0f, 0.0f, 0.0f);
	vertices[9].normal    = glm::vec3(-1.0f, 0.0f, 0.0f);
	vertices[10].normal   = glm::vec3(-1.0f, 0.0f, 0.0f);
	vertices[11].normal   = glm::vec3(-1.0f, 0.0f, 0.0f);
	vertices[8].texcoord  = glm::vec2(1.0f, 1.0f);
	vertices[9].texcoord  = glm::vec2(1.0f, 0.0f);
	vertices[10].texcoord = glm::vec2(0.0f, 1.0f);
	vertices[11].texcoord = glm::vec2(0.0f, 0.0f);
	// right
	vertices[12].position = glm::vec3(len, len, len);
	vertices[13].position = glm::vec3(len, -len, len);
	vertices[14].position = glm::vec3(len, len, -len);
	vertices[15].position = glm::vec3(len, -len, -len);
	vertices[12].normal   = glm::vec3(1.0f, 0.0f, 0.0f);
	vertices[13].normal   = glm::vec3(1.0f, 0.0f, 0.0f);
	vertices[14].normal   = glm::vec3(1.0f, 0.0f, 0.0f);
	vertices[15].normal   = glm::vec3(1.0f, 0.0f, 0.0f);
	vertices[12].texcoord = glm::vec2(0.0f, 1.0f);
	vertices[13].texcoord = glm::vec2(0.0f, 0.0f);
	vertices[14].texcoord = glm::vec2(1.0f, 1.0f);
	vertices[15].texcoord = glm::vec2(1.0f, 0.0f);
	// top
	vertices[16].position = glm::vec3(len, len, len);
	vertices[17].position = glm::vec3(-len, len, len);
	vertices[18].position = glm::vec3(len, len, -len);
	vertices[19].position = glm::vec3(-len, len, -len);
	vertices[16].normal   = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[17].normal   = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[18].normal   = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[19].normal   = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[16].texcoord = glm::vec2(1.0f, 0.0f);
	vertices[17].texcoord = glm::vec2(0.0f, 0.0f);
	vertices[18].texcoord = glm::vec2(1.0f, 1.0f);
	vertices[19].texcoord = glm::vec2(0.0f, 1.0f);
	// bottom
	vertices[20].position = glm::vec3(len, -len, len);
	vertices[21].position = glm::vec3(-len, -len, len);
	vertices[22].position = glm::vec3(len, -len, -len);
	vertices[23].position = glm::vec3(-len, -len, -len);
	vertices[20].normal	  = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices[21].normal	  = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices[22].normal	  = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices[23].normal	  = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices[20].texcoord = glm::vec2(1.0f, 1.0f);
	vertices[21].texcoord = glm::vec2(0.0f, 1.0f);
	vertices[22].texcoord = glm::vec2(1.0f, 0.0f);
	vertices[23].texcoord = glm::vec2(0.0f, 0.0f);

	std::vector<uint32_t> indices{
		0, 2, 3, 0, 3, 1, // front
		5, 6, 4, 5, 7, 6, // back
		11, 8, 10, 11, 9, 8, // left
		13, 14, 12, 13, 15, 14, // right
		17, 18, 19, 17, 16, 18, // top
		23, 20, 21, 23, 22, 20// bottom
	};

	return Mesh(vertices, indices, 0);

}

static constexpr float PI = 3.14159265358979323846;

// UV Sphere
// references:
// http://www.songho.ca/opengl/gl_sphere.html
Mesh Mesh::GenSphere(float scale, int lattDiv, int longDiv)
{
	
	std::vector<Vertex> vertices;
	const float invR = 1.0f / scale;
	const float sectorStep = 2 * PI / longDiv;  // divide along longitude
	const float stackStep = PI / lattDiv;       // divide along lattitude

	for (int i = 0; i <= lattDiv; ++i) {
		float a = PI / 2 - i * stackStep;
		float xy = scale * cosf(a);
		float z = scale * sinf(a);

		for (int j = 0; j <= longDiv; ++j) {
			float b = j * sectorStep;

			Vertex v;

// NOTE: 
// the north and south pole is at Z-axis, means that
// after mvp transform, the north pole towards outside of screen (u see it directly) 
// along with the texture squeeze/distortion(is avoidable?)
// AND there is a seam at xOz,x>0 (due to vertex where j == 0 and j == longDiv)
// maybe more texture sample point fix it, like bilinear
			//v.position.x = xy * cosf(b);
			//v.position.y = xy * sinf(b);
			//v.position.z = z;
// here change the orientation
// -x->z, -y->x, z->y
			v.position.x = -xy * sinf(b);
			v.position.y = z;
			v.position.z = -xy * cosf(b);

			v.normal.x = v.position.x * invR;   // use sphere normal instead of face normal (for smooth)
			v.normal.y = v.position.y * invR;
			v.normal.z = v.position.z * invR;
			
			v.texcoord.x = j * 1.0f / longDiv;
			v.texcoord.y = 1 - i * 1.0f / lattDiv;  // cmp to songho's, we don't want to flip uv
		
			vertices.push_back(v);
		}
	}

// generate CCW index list of sphere triangles
// k1--k1+1
// |  / |
// | /  |
// k2--k2+1
	std::vector<uint32_t> indices;
	for (int i = 0; i < lattDiv; ++i) {
		int k1 = i * (longDiv + 1);
		int k2 = k1 + (longDiv + 1);
		for (int j = 0; j < longDiv; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != lattDiv - 1) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	return Mesh(vertices, indices, 0);
}

static void GetIcosahedron(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
	const float phi = (1.0f + sqrtf(5.0f)) / 2;
	const float R = sqrtf(phi + 2); // circumradius 
	const float invR = 1.0f / R;
	vertices.resize(12);
	vertices[0].position = glm::vec3(-1, phi, 0);
	vertices[1].position = glm::vec3(1, phi, 0);
	vertices[2].position = glm::vec3(-1, -phi, 0);
	vertices[3].position = glm::vec3(1, -phi, 0);
	vertices[4].position = glm::vec3(0, -1, phi);
	vertices[5].position = glm::vec3(0, 1, phi);
	vertices[6].position = glm::vec3(0, -1, -phi);
	vertices[7].position = glm::vec3(0, 1, -phi);
	vertices[8].position = glm::vec3(phi, 0, -1);
	vertices[9].position = glm::vec3(phi, 0, 1);
	vertices[10].position = glm::vec3(-phi, 0, -1);
	vertices[11].position = glm::vec3(-phi, 0, 1);

	auto add = [&indices](int a, int b, int c) {
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	};
	indices.reserve(60);
	add(0, 11, 5);
	add(0, 5, 1);
	add(0, 1, 7);
	add(0, 7, 10);
	add(0, 10, 11);
	add(1, 5, 9);
	add(5, 11, 4);
	add(11, 10, 2);
	add(10, 7, 6);
	add(7, 1, 8);
	add(3, 9, 4);
	add(3, 4, 2);
	add(3, 2, 6);
	add(3, 6, 8);
	add(3, 8, 9);
	add(4, 9, 5);
	add(2, 4, 11);
	add(6, 2, 10);
	add(8, 6, 7);
	add(9, 8, 1);

	for (int i = 0; i < 12; ++i) {
		vertices[i].position *= invR;  // make circumsphere a unit sphere
	}

}


// Icosphere 
// references:
// generate
// http://www.songho.ca/opengl/gl_sphere.html programmatically generate, simple subdivision
// https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/ hard-code gen, cached subdivision
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html similar with 2nd [CURRENT]
// https://en.wikipedia.org/wiki/Regular_icosahedron misc
// https://observablehq.com/@mourner/fast-icosphere-mesh a js example
// uv mapping
// https://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere  zigzag
// https://gist.github.com/tnlogy/5146337  calcUV (need duplicate vertex=3*faces)
// https://mft-dev.dk/uv-mapping-sphere/   
// https://www.alexisgiard.com/icosahedron-sphere/ 
// https://github.com/cinder/Cinder/issues/1083 ! can calc uv at pixel shader( and have to update by orientation)
// https://github.com/kaiware007/IcoSphereCreator  https://www.binpress.com/creating-octahedron-sphere-unity/  manually fix
Mesh Mesh::GenSphere(float scale, int div)
{
	std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

	GetIcosahedron(vertices, indices);

	std::unordered_map<uint64_t, uint32_t> vcache;
	auto AddMidPoint = [&vertices, &vcache](uint64_t a, uint64_t b) -> uint32_t {
		if (a > b) std::swap(a, b);
		uint64_t key = (a << 32) + b;
		if (vcache.find(key) != vcache.end())
			return vcache[key];
		Vertex v;
		auto& pa = vertices[a].position;
		auto& pb = vertices[b].position;
		v.position = glm::normalize( (pa + pb) * 0.5f );
		vertices.push_back(v);
		vcache[key] = vertices.size() - 1;
		return vertices.size() - 1;
	};
	
	for (int i = 0; i < div; ++i) {
		std::vector<uint32_t> tmp_indices;
		for (size_t j = 0; j < indices.size(); j += 3) {
			//         0      
			//        / \       
			//     a *---* c
			//      / \ / \     
			//     1---*---2   
			//         b    
			uint32_t a = AddMidPoint(indices[j + 0], indices[j + 1]);
			uint32_t b = AddMidPoint(indices[j + 1], indices[j + 2]);
			uint32_t c = AddMidPoint(indices[j + 2], indices[j + 0]);

			tmp_indices.push_back(indices[j + 0]);
			tmp_indices.push_back(a);
			tmp_indices.push_back(c);

			tmp_indices.push_back(a);
			tmp_indices.push_back(indices[j + 1]);
			tmp_indices.push_back(b);

			tmp_indices.push_back(c);
			tmp_indices.push_back(b);
			tmp_indices.push_back(indices[j + 2]);

			tmp_indices.push_back(a);
			tmp_indices.push_back(b);
			tmp_indices.push_back(c);
		}
		indices = tmp_indices;
	}
	
	for (auto&& V : vertices) {
		V.normal = V.position;  // use sphere normal instead of face normal (for smooth)
// for now avoid to use icosphere with tex, remain much to do
		//wiki version  
		V.texcoord.x = 0.5f + atan2f(V.position.x , V.position.z) / (2 * PI);
		V.texcoord.y = 0.5f - asinf(V.position.y) / PI;
		V.position *= scale;
	}
	// std::cout << vertices.size() << '\n'; // 12 + 10 * (4 ** div - 1) https://pypi.org/project/icosphere/
	return Mesh(vertices, indices, 0);
}