#include "stdafx.h"

#include "scene.h"

namespace BeeView
{

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
		m_device = rtcNewDevice(NULL);
		m_rtcscene = rtcDeviceNewScene(m_device, RTC_SCENE_STATIC, RTC_INTERSECT1);

		struct Vertex { float x, y, z, a; }; // vertex coordinates, 'a' not used
		struct Triangle { int v0, v1, v2; }; // vertex indices


		 //std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\bee-eye-view\\models\\cornell\\cornell_box.obj";
		 //std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\bee-eye-view\\models\\test\\test.obj";
		 //std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\bee-eye-view\\models\\website\\website.obj";
		//std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\bee-eye-view\\models\\hessen\\panorama_skydome_triang_smooth.obj";
		std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\bee-eye-view\\models\\hessen\\skydome_minus_z_foward.obj";


		std::cout << "Loading Scene into Embree... " << std::endl << std::endl;
		std::cout << "Number of Objects in Scene: " << m_objects.size() << std::endl;

		for each (const std::shared_ptr<Mesh> &mesh in m_objects)
		{
			std::cout << std::endl;
			std::cout << "### Loading Mesh: " << mesh->texture->name << " ###" << std::endl;
			std::cout << std::endl;
			std::cout << "Number of Vertices in Mesh: " << mesh->numVertices() << std::endl;
			std::cout << "Number of Normals in Mesh: " << mesh->normals.size() << std::endl;
			std::cout << "Number of Faces in Mesh: " << mesh->triangles.size() << std::endl;
			std::cout << std::endl;

			// create the mesh
			unsigned int rtcmesh = rtcNewTriangleMesh(m_rtcscene, RTC_GEOMETRY_STATIC, mesh->triangles.size(), mesh->numVertices()); //n_tri: number of traingles, n_v: number of vertices

			Vertex* vertices = (Vertex*)rtcMapBuffer(m_rtcscene, rtcmesh, RTC_VERTEX_BUFFER);

			// fill vertices
			for (int i = 0; i < mesh->numVertices(); i++)
			{
				vertices[i].x = mesh->positions[i](0);
				vertices[i].y = mesh->positions[i](1);
				vertices[i].z = mesh->positions[i](2); // Fix left vs righthanded coordinate system
			}
			rtcUnmapBuffer(m_rtcscene, rtcmesh, RTC_VERTEX_BUFFER);


			// fill triangle indices
			Triangle* triangles = (Triangle*)rtcMapBuffer(m_rtcscene, rtcmesh, RTC_INDEX_BUFFER);

			//TODO check if simple copy (=) works
			for (int i = 0; i < mesh->triangles.size(); i++)
			{
				triangles[i].v0 = mesh->triangles[i].v0;
				triangles[i].v1 = mesh->triangles[i].v1;
				triangles[i].v2 = mesh->triangles[i].v2;
			}
			rtcUnmapBuffer(m_rtcscene, rtcmesh, RTC_INDEX_BUFFER);

		}

		//after adding all meshes call
		/* commit changes to scene */
		rtcCommit(m_rtcscene);
		
		std::cout << std::endl << "Finished loading Scene into Embree." << std::endl << std::endl;

		return;
	}

	void Scene::cleanupEmbree()
	{
		
			// cleanup
			rtcDeleteScene(m_rtcscene);
			m_rtcscene = nullptr;
			rtcDeleteDevice(m_device);
			m_device = nullptr;

			return;
	}
}


