#pragma once

#include <vector>
#include <memory>
#include <embree2\rtcore.h>

//#include "vec.h"
#include "utility.h"
#include "texture.h"

namespace BeeView {

	struct Triangle
	{
		int v0, v1, v2;

		Triangle() : v0(0), v1(0), v2(0) { }
		Triangle(int x) : v0(x), v1(x), v2(x) { }
		Triangle(int x, int y, int z) : v0(x), v1(y), v2(z) { }
	};

	class Mesh 
	{
	public:
		unsigned int id; // embree object id?
		std::vector<Vec3f> positions; // vertex coordinates, length = numVertices
		std::vector<Vec2f> texcoords; // vertex texture coordinates, length = numVertices

		std::vector<Vec3f> normals; // don't need them for beeview, but maybe for other cool, fancy stuff

		std::vector<Triangle> triangles;

		std::shared_ptr<Texture> texture;

		Mesh();

		size_t numVertices() const 
		{
			return positions.size();
		}
	};

	class Scene
	{
	public:
		RTCScene rtcscene; // embree scene
		std::vector<std::shared_ptr<Mesh>> objects;

		Scene();

		void addObject(std::shared_ptr<Mesh> mesh);
	};

}


