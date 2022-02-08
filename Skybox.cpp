#include "Skybox.h"

#include <filesystem>
#include <iostream>

namespace {
	constexpr int NFACE = 6;
	const char* face_name[] = { "top", "bottom", "left", "right", "front", "back" };
	constexpr int NFORMAT = 4;
	const char* format[] = { "jpg", "png", "bmp", "tga"};  // depends on stb_image 
}


Skybox::Skybox(const char* dir)
	: mesh(Mesh::GenCube(2.0f))
{
	using namespace std::filesystem;
	faces.resize(6);

	path str(dir);
	if (!exists(str) && !is_directory(str)) {
		std::cout << "skybox path error!\n";
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NFACE; ++i) {
		str.replace_filename(face_name[i]);
		bool ok = false;
		for (int j = 0; j < NFORMAT; ++j) {
			str.replace_extension(format[j]);
			if (exists(str) && is_regular_file(str)) {
				ok = true;
				break;
			}
		}
		if (ok) {
			faces[i] = Texture2D(str.string().c_str());
			faces[i].wmode = Texture2D::WrapMode::ClampToEdge;
			std::cout << str.stem().string() << " load!\n";
		}
		else {
			std::cout << str.stem().string() << " MISS!\n";
			exit(EXIT_FAILURE);
		}
	}

	std::cout << "skybox load!\n";
}

glm::vec4 Skybox::Sample(float x, float y, float z)
{
	float absX = fabs(x);
	float absY = fabs(y);
	float absZ = fabs(z);
	float ma, sc, tc;
	int faceIndex;
	ma = std::max(std::max(absX, absY), absZ);
	if (ma == absX) {
		if (x > 0) {
			faceIndex = 3;   // right
			sc = z;
			tc = y;
		}
		else {
			faceIndex = 2;   // left
			sc = -z;
			tc = y;
		}
	}
	else if (ma == absY) {
		if (y > 0) {
			faceIndex = 0;   // top
			sc = x;
			tc = z;
		}
		else {
			faceIndex = 1;   // bottom
			sc = x;
			tc = -z;
		}
	}
	else {
		if (z > 0) {
			faceIndex = 5;  // back
			sc = -x;
			tc = y;
		}
		else {
			faceIndex = 4;    // front
			sc = x;
			tc = y;
		}
	}
	glm::vec2 uv((sc / ma + 1) / 2, (tc / ma + 1) / 2);
	return faces[faceIndex].Sample(uv.x, uv.y);
}
