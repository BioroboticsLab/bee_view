#pragma once

#include "scene.h"

namespace BeeView
{
	/* create a scene from an .obj file */
	std::shared_ptr<Scene> loadOBJ(const std::string fileName, bool leftHanded = false);
}