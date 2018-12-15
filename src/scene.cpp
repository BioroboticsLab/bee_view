#include "stdafx.h"

#include "scene.h"

namespace BeeView
{
	extern int verbose_lvl;

	Mesh::Mesh()
	{
		texture = std::make_shared<Texture>(Texture());
	}

	void Scene::addObject(std::shared_ptr<Mesh> mesh) 
	{
		m_objects.push_back(mesh);
		mesh->id = static_cast<unsigned int>(m_objects.size()-1);
	}

	/* commit all objects to embree */
	void Scene::initEmbree()
	{
		m_device = rtcNewDevice("verbose=3");
		m_rtcscene = rtcNewScene(m_device);

  		rtcSetSceneFlags(m_rtcscene,RTC_SCENE_FLAG_COMPACT);
  		rtcSetSceneBuildQuality(m_rtcscene,RTC_BUILD_QUALITY_MEDIUM); 

		struct Vertex { float x, y, z, a; }; // vertex coordinates, 'a' not used
		struct Triangle { int v0, v1, v2; }; // vertex indices

		if (verbose_lvl > 0)
			std::cout << std::endl << "Loading Scene into Embree... ";

		size_t total_verts = 0;
		size_t total_triangles = 0;
		for(const std::shared_ptr<Mesh> &mesh : m_objects)
		{
			total_triangles += mesh->triangles.size();
			total_verts += mesh->numVertices();

			if (verbose_lvl > 2)
			{
				std::cout << std::endl;
				std::cout << "Loading Mesh: " << mesh->texture->name << "..." << std::endl;
				std::cout << std::endl;
				std::cout << "Number of Vertices in Mesh: " << mesh->numVertices() << std::endl;
				std::cout << "Number of Normals in Mesh: " << mesh->normals.size() << std::endl;
				std::cout << "Number of Faces in Mesh: " << mesh->triangles.size() << std::endl;
				std::cout << std::endl;
			}

			// create the mesh
   			RTCGeometry geom_0 = rtcNewGeometry (m_device, RTC_GEOMETRY_TYPE_TRIANGLE);
   			rtcSetGeometryBuildQuality(geom_0,RTC_BUILD_QUALITY_MEDIUM);
   			rtcSetGeometryTimeStepCount(geom_0,1);	

			Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geom_0,RTC_BUFFER_TYPE_VERTEX,0,RTC_FORMAT_FLOAT3,4*sizeof(float),mesh->numVertices());

			// fill vertices
			for (size_t i = 0; i < mesh->numVertices(); i++)
			{
				vertices[i].x = mesh->positions[i](0);
				vertices[i].y = mesh->positions[i](1);
				vertices[i].z = mesh->positions[i](2); // Fix left vs righthanded coordinate system
			}
			
			// fill triangle indices
			Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geom_0,RTC_BUFFER_TYPE_INDEX,0,RTC_FORMAT_UINT3,3*sizeof(int),mesh->triangles.size());


			for (unsigned int i = 0; i < mesh->triangles.size(); i++)
			{
				triangles[i].v0 = mesh->triangles[i].v0;
				triangles[i].v1 = mesh->triangles[i].v1;
				triangles[i].v2 = mesh->triangles[i].v2;
			}

			rtcCommitGeometry(geom_0); //?
   			rtcAttachGeometry(m_rtcscene,geom_0);
   			rtcReleaseGeometry(geom_0); 
		}

		//after adding all meshes call
		/* commit changes to scene */
		rtcCommitScene(m_rtcscene);
		
		// calc AABB
		rtcGetSceneBounds(m_rtcscene,&m_bounds);

		if (verbose_lvl > 0)
			std::cout << "Done." << std::endl;
		if (verbose_lvl > 1) {
			std::cout << "Scene stats: " << total_triangles << " triangles and " << total_verts << " vertices." << std::endl;
		}
		return;
	}

	void Scene::cleanupEmbree()
	{
			if (verbose_lvl > 0)
				std::cout << std::endl <<"Cleaning up Embree... ";
			// cleanup
			std::cout << "Embree: " << rtcGetDeviceError(m_device) << std::endl;
			rtcReleaseScene(m_rtcscene);
			m_rtcscene = nullptr;
			rtcReleaseDevice (m_device);
			m_device = nullptr;

			if (verbose_lvl > 0)
				std::cout << "Done." << std::endl;
			return;
	}
}


