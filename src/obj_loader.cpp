// Modified obj loader from intels Embree, which is based on TinyObjLoader v0.9 by syoyo https://github.com/syoyo/tinyobjloader

// Contributors:
// Intel Corporation
// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "stdafx.h"

#include "scene.h"
#include "obj_loader.h"
#include "utility.h"

namespace BeeView
{
	extern int verbose_lvl;

	/*! Three-index vertex, indexing start at 0, -1 means invalid vertex. */
	struct Vertex {
		int v, vt, vn;
		Vertex() {};
		Vertex(int v) : v(v), vt(v), vn(v) {};
		Vertex(int v, int vt, int vn) : v(v), vt(vt), vn(vn) {};
	};

	static inline bool operator < (const Vertex& a, const Vertex& b) {
		if (a.v != b.v)  return a.v < b.v;
		if (a.vn != b.vn) return a.vn < b.vn;
		if (a.vt != b.vt) return a.vt < b.vt;
		return false;
	}

	/*! Fill space at the end of the token with 0s. */
	static inline const char* trimEnd(const char* token) {
		size_t len = strlen(token);
		if (len == 0) return token;
		char* pe = (char*)(token + len - 1);
		while ((*pe == ' ' || *pe == '\t' || *pe == '\r') && pe >= token) *pe-- = 0;
		return token;
	}

	/*! Determine if character is a separator. */
	static inline bool isSep(const char c) {
		return (c == ' ') || (c == '\t');
	}

	/*! Parse separator. */
	static inline const char* parseSep(const char*& token) {
		size_t sep = strspn(token, " \t");
		if (!sep) std::cout <<"separator expected";
		return token += sep;
	}

	/*! Parse optional separator. */
	static inline const char* parseSepOpt(const char*& token) {
		return token += strspn(token, " \t");
	}

	/*! Read float from a string. */
	static inline float getFloat(const char*& token) {
		token += strspn(token, " \t");
		float n = (float)atof(token);
		token += strcspn(token, " \t\r");
		return n;
	}

	/*! Read int from a string. */
	static inline int getInt(const char*& token) {
		token += strspn(token, " \t");
		int n = atoi(token);
		token += strcspn(token, " \t\r");
		return n;
	}

	/*! Read Vec2f from a string. */
	static inline Vec2f getVec2f(const char*& token) {
		float x = getFloat(token);
		float y = getFloat(token);
		return Vec2f(x, y);
	}

	/*! Read Vec3f from a string. */
	static inline Vec3f getVec3f(const char*& token) {
		float x = getFloat(token);
		token += strspn(token, " \t");
		if (*token == 0) return Vec3f(x,x,x);
		float y = getFloat(token);
		float z = getFloat(token);
		return Vec3f(x, y, z);
	}


	class OBJLoader
	{
	public:

		/*! Constructor. */
		OBJLoader(const std::string fileName);

		/*! output model */
		std::shared_ptr<Scene> group;

	private:

		/*! file to load */
		const std::string path;

		/*! Geometry buffer. never gets cleared */
		std::vector<Vec3f> v; // positions
		std::vector<Vec3f> vn; // vector-normals
		std::vector<Vec2f> vt; // texture uv coordinates

		std::vector<std::vector<Vertex> > curGroup; // the face buffer, gets cleared after each object
			
		/* material handling */
		std::shared_ptr<Texture >  curMaterial;

		/* maps material name to Texture */
		std::map<std::string, std::shared_ptr<Texture>> textureMap;


	private:
		void loadMTL(const std::string& fileName);
		int fix_v(int index);
		int fix_vt(int index);
		int fix_vn(int index);
		void flushFaceGroup();
		Vertex getInt3(const char*& token);
		uint32_t getVertex(std::map<Vertex, uint32_t>& vertexMap, std::shared_ptr<Mesh> mesh, const Vertex& i);

	};

	OBJLoader::OBJLoader(const std::string fileName)
		: group(std::make_shared<Scene>(Scene())), path(fileName)
	{
		if (verbose_lvl > 0)
			std::cout << "Loading " << fileName << "... " << std::endl;

		/* open file */
		std::ifstream cin;
		cin.open(fileName.c_str());
		if (!cin.is_open()) {
			std::cerr << "cannot open " + fileName;
			return;
		}

		char line[10000];
		memset(line, 0, sizeof(line));

		// default material is empty texture
		std::shared_ptr<Texture> defaultMaterial = std::make_shared<Texture>(Texture());;
		curMaterial = defaultMaterial;

		while (cin.peek() != -1)
		{
			/* load next multiline */
			char* pline = line;
			while (true) {
				cin.getline(pline, sizeof(line) - (pline - line) - 16, '\n');
				ssize_t last = strlen(pline) - 1;
				if (last < 0 || pline[last] != '\\') break;
				pline += last;
				*pline++ = ' ';
			}

			const char* token = trimEnd(line + strspn(line, " \t"));
			if (token[0] == 0) continue;

			/*! parse position */
			if (token[0] == 'v' && isSep(token[1])) {
				v.push_back(getVec3f(token += 2)); continue;
			}

			/* parse normal */
			if (token[0] == 'v' && token[1] == 'n' && isSep(token[2])) {
				vn.push_back(getVec3f(token += 3));
				continue;
			}

			/* parse texcoord */
			if (token[0] == 'v' && token[1] == 't' && isSep(token[2])) { vt.push_back(getVec2f(token += 3)); continue; }

			/*! parse face */
			if (token[0] == 'f' && isSep(token[1]))
			{
				parseSep(token += 1);

				std::vector<Vertex> face;
				while (token[0]) {
					Vertex vtx = getInt3(token);
					face.push_back(vtx);
					parseSepOpt(token);
				}
				curGroup.push_back(face);
				continue;
			}

			/*! use material, usemtl defines material for the following faces */
			if (!strncmp(token, "usemtl", 6) && isSep(token[6]))
			{
				flushFaceGroup(); // add previous object to scene
				std::string name(parseSep(token += 6));

				if(verbose_lvl > 2)
					std::cout << std::endl << "usemtl " << name << std::endl;

				// if no material exists
				if (textureMap.find(name) == textureMap.end()) {
					if (verbose_lvl > 2)
						std::cout << "No material found, useing default material. " << std::endl;
					curMaterial = defaultMaterial;
				}
				else 
				{
					curMaterial = textureMap[name];
				}

				continue;
			}

			/* load material library */
			if (!strncmp(token, "mtllib", 6) && isSep(token[6])) {
				
				loadMTL(getFilePath(path) + std::string(parseSep(token += 6)));
				continue;
			}

			// ignore unknown stuff
		}

		flushFaceGroup();

		cin.close();

		if (verbose_lvl > 0)
			std::cout << "Done. " << std::endl;
	}

	/*! handles relative indices and starts indexing from 0 */
	int OBJLoader::fix_v(int index) { return (index > 0 ? index - 1 : (index == 0 ? 0 : (int)v.size() + index)); }
	int OBJLoader::fix_vt(int index) { return (index > 0 ? index - 1 : (index == 0 ? 0 : (int)vt.size() + index)); }
	int OBJLoader::fix_vn(int index) { return (index > 0 ? index - 1 : (index == 0 ? 0 : (int)vn.size() + index)); }

	/*! Parse differently formated triplets like: n0, n0/n1/n2, n0//n2, n0/n1.          */
	/*! All indices are converted to C-style (from 0). Missing entries are assigned -1. */
	Vertex OBJLoader::getInt3(const char*& token)
	{
		Vertex v(-1);
		v.v = fix_v(atoi(token));
		token += strcspn(token, "/ \t\r");
		if (token[0] != '/') return(v);
		token++;

		// it is i//n
		if (token[0] == '/') {
			token++;
			v.vn = fix_vn(atoi(token));
			token += strcspn(token, " \t\r");
			return(v);
		}

		// it is i/t/n or i/t
		v.vt = fix_vt(atoi(token));
		token += strcspn(token, "/ \t\r");
		if (token[0] != '/') return(v);
		token++;

		// it is i/t/n
		v.vn = fix_vn(atoi(token));
		token += strcspn(token, " \t\r");
		return(v);
	}

	/*! prevents vertex duplicates and aligns position and texture buffer */
	uint32_t OBJLoader::getVertex(std::map<Vertex, uint32_t>& vertexMap, std::shared_ptr<Mesh> mesh, const Vertex& i)
	{
		// prevent duplicates with map
		const std::map<Vertex, uint32_t>::iterator& entry = vertexMap.find(i);
		if (entry != vertexMap.end()) return(entry->second);
		mesh->positions.push_back(Vec3f(v[i.v](0), v[i.v](1), v[i.v](2))); // eigen vec notation
		//mesh.positions.push_back(Vec3f(v[i.v].x, v[i.v].y, v[i.v].z)); // my vec notation


		// make vt and positions buffer same size
		if (i.vt >= 0) {
			while (mesh->texcoords.size() < mesh->positions.size()) mesh->texcoords.push_back(Vec2f::Zero()); // some vertices might not have a texture coordinate
			
			//while (mesh.texcoords.size() < mesh.positions.size()) mesh.texcoords.push_back(0); // some vertices might not have a texture coordinate

			mesh->texcoords[mesh->positions.size() - 1] = vt[i.vt];
		}

		/*
		if (i.vn >= 0) { // XXXNORMALS
			while (mesh.normals.size() < mesh.positions.size()) mesh.normals.push_back(Vec3f::Zero()); // some vertices might not have a normal
			mesh.normals[mesh.positions.size() - 1] = vn[i.vn];
		}
		*/

		return(vertexMap[i] = int(mesh->positions.size()) - 1);
	}

	/*! end current facegroup and append to mesh 
		builds a vertex map to prevent duplicates (global v,vt,vn buffer is shared by all m_objects)
	*/
	void OBJLoader::flushFaceGroup()
	{

		if (curGroup.empty()) return;

		if (verbose_lvl > 2)
			std::cout << "Begin Flushing facegroup: " << curMaterial->name << "... ";

		std::shared_ptr<Mesh> mesh = std::make_unique<Mesh>(Mesh());

		// merge three indices into one
		std::map<Vertex, uint32_t> vertexMap;
		for (size_t j = 0; j < curGroup.size(); j++)
		{
			/* iterate over all faces */
			const std::vector<Vertex>& face = curGroup[j];

			/* triangulate the face with a triangle fan */
			Vertex i0 = face[0], i1 = Vertex(-1), i2 = face[1];
			for (size_t k = 2; k < face.size(); k++)
			{
				i1 = i2; i2 = face[k];
				uint32_t v0, v1, v2;
				v0 = getVertex(vertexMap, mesh, i0); //get vertex also adds verices and textures (and normals) to scene
				v1 = getVertex(vertexMap, mesh, i1);
				v2 = getVertex(vertexMap, mesh, i2);
				
				mesh->triangles.push_back(Triangle(v0, v1, v2));
			}

			/* there may be vertices without texture coordinates, thus we have to make this array the same size here */
			if (mesh->texcoords.size()) while (mesh->texcoords.size() < mesh->numVertices()) mesh->texcoords.push_back(Vec2f::Zero()); // TODO: optimize: Zero doesnt need to be created can be referenz to const global zero object?
			/* same for normals*/
			//if (mesh.normals.size()) while (mesh.normals.size() < mesh.numVertices()) mesh.normals.push_back(Vec3f::Zero()); // XXXNORMALS

			//if (mesh.texcoords.size()) while (mesh.texcoords.size() < mesh.numVertices()) mesh.texcoords.push_back(0);

		}

		curGroup.clear();
		mesh->texture = curMaterial;
		group->addObject(mesh);

		if (verbose_lvl > 2)
			std::cout << "Done. " << std::endl;
	}

	/* load material file */
	void OBJLoader::loadMTL(const std::string &fileName)
	{
		if (verbose_lvl > 2)
			std::cout << "Reading " << fileName << "... ";

		std::ifstream cin;
		cin.open(fileName.c_str());
		if (!cin.is_open()) {
			std::cerr << "cannot open " << fileName << std::endl;
			return;
		}

		char line[10000];
		memset(line, 0, sizeof(line));

		std::string name;
		Texture cur;

		while (cin.peek() != -1)
		{
			/* load next multiline */
			char* pline = line;
			while (true) {
				cin.getline(pline, sizeof(line) - (pline - line) - 16, '\n');
				ssize_t last = strlen(pline) - 1;
				if (last < 0 || pline[last] != '\\') break;
				pline += last;
				*pline++ = ' ';
			}
			const char* token = trimEnd(line + strspn(line, " \t"));

			if (token[0] == 0) continue; // ignore empty lines
			if (token[0] == '#') continue; // ignore comments

			/* add texture to texturemap if map_Ka or map_kd */
			if (!strncmp(token, "newmtl", 6))
			{
				// if not first newmtl: add previous mtl to texturemap
				if (name != "")
					textureMap[name] = std::make_shared<Texture>(cur);

				/* get material name */
				parseSep(token += 6);
				name = token;
				cur = Texture();
				cur.name = name;
				continue;
			}
			if (name == "") std::cerr << "invalid material file: newmtl expected first";

			try
			{
				
				if (!strncmp(token, "map_Ka", 6) || !strncmp(token, "Ka_map", 6)) { 
					parseSep(token += 6);
					cur.loadImage(token);
					continue; 
				}
				if (!strncmp(token, "map_Kd", 6) || !strncmp(token, "Kd_map", 6)) {
					parseSep(token += 6);
					cur.loadImage(token);
					continue;
				}

				if (!strncmp(token, "Ka", 2)) { parseSep(token += 2);  cur.Ka = getVec3f(token); continue; }
				if (!strncmp(token, "Kd", 2)) { parseSep(token += 2);  cur.Kd = getVec3f(token); continue; }
			
			}
			catch (const std::runtime_error e) {
				std::cerr << "Error: " << e.what() << std::endl;
			}

			// ignore everything else
		}

		// add last mtl to texturemap
		if (name != "")
			textureMap[name] = std::make_shared<Texture>(cur);

		cin.close();

		if (verbose_lvl > 2)
			std::cout << "Done." << std::endl;

		return;
	}


	std::shared_ptr<Scene> loadOBJ(const std::string fileName) {
		OBJLoader loader(fileName);
		return loader.group;
	}
}