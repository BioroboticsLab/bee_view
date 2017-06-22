#pragma once

#include "scene.h"

namespace BeeView
{
	std::shared_ptr<Scene> loadOBJ(const std::string fileName);
}