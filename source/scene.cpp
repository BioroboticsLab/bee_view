#include "../build/BeeView/BeeView/stdafx.h"


#include "scene.h"


//extern RTCDevice g_device;

namespace BeeView
{

	Mesh::Mesh()
	{
		texture = std::make_shared<Texture>(Texture());
	}

	Scene::Scene()
	{
		//rtcscene = rtcDeviceNewScene(g_device, RTC_SCENE_STATIC, RTC_INTERSECT1);
	}

	void Scene::addObject(std::shared_ptr<Mesh> mesh) 
	{
		objects.push_back(mesh);
		mesh->id = static_cast<unsigned int>(objects.size()-1);
	}
}
