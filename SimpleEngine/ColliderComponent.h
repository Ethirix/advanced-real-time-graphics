#pragma once
#include "ComponentBase.h"

class GameObject;
class ColliderComponent : ComponentBase
{
public:
	explicit ColliderComponent(WP_GAMEOBJECT owningGameObject);
};
